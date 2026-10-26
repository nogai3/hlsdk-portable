#include "hud.h"
#include "cl_util.h"
#include "vgui_BuyMenu.h"
#include "vgui_TeamFortressViewport.h"
#include <VGUI_FileInputStream.h>
#include <stdio.h>
#include <string.h>

using namespace vgui;

#define BUY_WIDE   600
#define BUY_TALL   440
#define BUY_S(v)   ((int)((v) * (float)ScreenHeight / 480.0f))

#define CLR_ORANGE   255, 170, 0

#define BUY_LOGO_PATH   "gfx/vgui/ct_logo.tga"

#define BUY_FONT_TITLE  "Title Font"
#define BUY_FONT_BUTTON "Primary Button Text"
#define BUY_FONT_TEXT   "Briefing Text"

static const char *g_szCategoryNames[8] =
{
    "1  PISTOLS",
    "2  SHOTGUNS",
    "3  SMGs",
    "4  RIFLES",
    "5  MACHINE GUNS",
    "6  PRIMARY AMMO",
    "7  SECONDARY AMMO",
    "8  EQUIPMENT"
};

static const char *g_szStatNames[BUY_STAT_ROWS] =
{
    "PRICE",
    "COUNTRY",
    "CALIBER",
    "MAGAZINE CAPACITY",
    "FIRERATE",
    "WEIGHT (with empty magazine)",
    "BULLET WEIGHT",
    "INITIAL BULLET SPEED",
    "INITIAL BULLET ENERGY"
};

static vgui::Font *BuyMenu_GetFont(const char *schemeName)
{
    if (!gViewPort)
        return NULL;

    CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
    if (!pSchemes)
        return NULL;

    SchemeHandle_t h = pSchemes->getSchemeHandle(schemeName);
    return pSchemes->getFont(h);
}

static vgui::BitmapTGA *BuyMenu_LoadTGA(const char *path)
{
    if (!path || !path[0])
        return NULL;

    byte *pFile = gEngfuncs.COM_LoadFile((char *)path, 5, NULL);
    if (!pFile)
    {
        gEngfuncs.Con_Printf("BuyMenu: file not found: %s\n", path);
        return NULL;
    }
    gEngfuncs.COM_FreeFile(pFile);

    vgui::FileInputStream *fis = new vgui::FileInputStream(path, false);
    vgui::BitmapTGA *tga = new vgui::BitmapTGA(fis, true);
    fis->close();
    delete fis;
    return tga;
}

CBuyLabel::CBuyLabel(const char *text, int x, int y, int w, int h, vgui::Font *font, int r, int g, int b)
    : vgui::Panel(x, y, w, h)
{
    m_pFont = font;
    m_iR = r;
    m_iG = g;
    m_iB = b;
    m_szText[0] = 0;
    setPaintBackgroundEnabled(false);
    SetText(text);
}

void CBuyLabel::SetText(const char *text)
{
    strncpy(m_szText, text ? text : "", sizeof(m_szText) - 1);
    m_szText[sizeof(m_szText) - 1] = 0;
    repaint();
}

void CBuyLabel::SetTextColor(int r, int g, int b)
{
    m_iR = r;
    m_iG = g;
    m_iB = b;
    repaint();
}

void CBuyLabel::paint()
{
    int len = (int)strlen(m_szText);
    if (len <= 0)
        return;

    int w, t;
    getPaintSize(w, t);

    int th = t / 2;
    if (m_pFont)
    {
        drawSetTextFont(m_pFont);
        th = m_pFont->getTall();
    }
    else
    {
        drawSetTextFont(Scheme::sf_primary1);
    }

    drawSetTextColor(m_iR, m_iG, m_iB, 0);
    drawSetTextPos(0, (t - th) / 2);
    drawPrintText(m_szText, len);
}

CBuyButton::CBuyButton(const char *text, int x, int y, int w, int h, vgui::Font *font)
    : vgui::Button("", x, y, w, h)
{
    m_pFont = font;
    m_iR = 255;
    m_iG = 170;
    m_iB = 0;
    m_iInset = BUY_S(6);
    m_szText[0] = 0;
    SetCaption(text);
}

void CBuyButton::SetCaption(const char *text)
{
    strncpy(m_szText, text ? text : "", sizeof(m_szText) - 1);
    m_szText[sizeof(m_szText) - 1] = 0;
    repaint();
}

void CBuyButton::SetTextColor(int r, int g, int b)
{
    m_iR = r;
    m_iG = g;
    m_iB = b;
    repaint();
}

void CBuyButton::paintBackground()
{
    int w, t;
    getPaintSize(w, t);
    drawSetColor(0, 0, 0, 40);
    drawFilledRect(0, 0, w, t);
}

void CBuyButton::paint()
{
    int len = (int)strlen(m_szText);
    if (len <= 0)
        return;

    int w, t;
    getPaintSize(w, t);

    int th = t / 2;
    if (m_pFont)
    {
        drawSetTextFont(m_pFont);
        th = m_pFont->getTall();
    }
    else
    {
        drawSetTextFont(Scheme::sf_primary1);
    }

    drawSetTextColor(m_iR, m_iG, m_iB, 0);
    drawSetTextPos(m_iInset, (t - th) / 2);
    drawPrintText(m_szText, len);
}

CBuyPanelBase::CBuyPanelBase() : vgui::Panel(0, 0, BUY_S(BUY_WIDE), BUY_S(BUY_TALL))
{
    m_iNumButtons = 0;
    m_pLogo = NULL;
    m_pTitle = NULL;

    for (int i = 0; i < BUY_MAX_BUTTONS; i++)
    {
        m_pButtons[i] = NULL;
        m_bEnabled[i] = true;
    }

    m_pBorder[BTN_NORMAL]   = new LineBorder(Color(150, 100, 15, 0));
    m_pBorder[BTN_HOVER]    = new LineBorder(Color(255, 170, 0, 0));
    m_pBorder[BTN_DISABLED] = new LineBorder(Color(70, 50, 10, 0));

    Reposition();
}

void CBuyPanelBase::Reposition()
{
    setPos((ScreenWidth - BUY_S(BUY_WIDE)) / 2, (ScreenHeight - BUY_S(BUY_TALL)) / 2);
}

void CBuyPanelBase::BuildFrame(const char *title)
{
    setPaintBackgroundEnabled(true);
    setBgColor(0, 0, 0, 60);
    setBorder(null);

    m_pLogo = new Label("", BUY_S(10), BUY_S(4), BUY_S(44), BUY_S(44));
    m_pLogo->setParent(this);
    m_pLogo->setPaintBackgroundEnabled(false);
    m_pLogo->setContentAlignment(Label::a_center);
    m_pLogo->setImage(BuyMenu_LoadTGA(BUY_LOGO_PATH));

    m_pTitle = AddLabel(title, 56, 14, 500, 24, BUY_FONT_TITLE, CLR_ORANGE);
    AddLine(0, 52, BUY_WIDE, 1, 60, 45, 10);
}

CBuyButton *CBuyPanelBase::AddButton(const char *text, int x, int y, int w, int h)
{
    if (m_iNumButtons >= BUY_MAX_BUTTONS)
        return NULL;

    CBuyButton *b = new CBuyButton(text, BUY_S(x), BUY_S(y), BUY_S(w), BUY_S(h), BuyMenu_GetFont(BUY_FONT_BUTTON));
    b->setParent(this);
    b->addActionSignal(this);
    b->addInputSignal(this);

    m_pButtons[m_iNumButtons] = b;
    m_bEnabled[m_iNumButtons] = true;
    ApplyStyle(m_iNumButtons, BTN_NORMAL);
    m_iNumButtons++;
    return b;
}

CBuyLabel *CBuyPanelBase::AddLabel(const char *text, int x, int y, int w, int h,
                                   const char *fontScheme, int r, int g, int b)
{
    CBuyLabel *l = new CBuyLabel(text, BUY_S(x), BUY_S(y), BUY_S(w), BUY_S(h), BuyMenu_GetFont(fontScheme), r, g, b);
    l->setParent(this);
    return l;
}

vgui::Panel *CBuyPanelBase::AddLine(int x, int y, int w, int h, int r, int g, int b)
{
    Panel *p = new Panel(BUY_S(x), BUY_S(y), BUY_S(w), BUY_S(h));
    p->setParent(this);
    p->setPaintBackgroundEnabled(true);
    p->setBgColor(r, g, b, 0);
    return p;
}

void CBuyPanelBase::ApplyStyle(int i, int state)
{
    CBuyButton *b = m_pButtons[i];
    if (!b)
        return;

    switch (state)
    {
    case BTN_HOVER:
        b->SetTextColor(255, 220, 120);
        b->setBorder(m_pBorder[BTN_HOVER]);
        break;
    case BTN_DISABLED:
        b->SetTextColor(110, 75, 0);
        b->setBorder(m_pBorder[BTN_DISABLED]);
        break;
    default:
        b->SetTextColor(CLR_ORANGE);
        b->setBorder(m_pBorder[BTN_NORMAL]);
        break;
    }
    b->repaint();
}

void CBuyPanelBase::SetButtonEnabled(int i, bool enabled)
{
    if (i < 0 || i >= m_iNumButtons)
        return;
    m_bEnabled[i] = enabled;
    ApplyStyle(i, enabled ? BTN_NORMAL : BTN_DISABLED);
}

int CBuyPanelBase::FindButton(vgui::Panel *panel)
{
    for (int i = 0; i < m_iNumButtons; i++)
    {
        if (m_pButtons[i] == panel)
            return i;
    }
    return -1;
}

void CBuyPanelBase::actionPerformed(vgui::Panel *panel)
{
    int i = FindButton(panel);
    if (i >= 0)
        OnButtonPressed(i);
}

void CBuyPanelBase::cursorEntered(vgui::Panel *panel)
{
    int i = FindButton(panel);
    if (i < 0)
        return;

    if (m_bEnabled[i])
        ApplyStyle(i, BTN_HOVER);
    OnButtonHover(i);
}

void CBuyPanelBase::cursorExited(vgui::Panel *panel)
{
    int i = FindButton(panel);
    if (i < 0)
        return;

    ApplyStyle(i, m_bEnabled[i] ? BTN_NORMAL : BTN_DISABLED);
}

void CBuyPanelBase::Close()
{
    setVisible(false);
    if (gViewPort)
        gViewPort->UpdateCursorState();
}

CBuyMenu::CBuyMenu()
{
    m_pCategory = NULL;

    BuildFrame("BUY MENU");

    m_pLblMoney = AddLabel("", 180, 18, 200, 18, BUY_FONT_TEXT, CLR_ORANGE);
    m_pLblSubtitle = AddLabel("SHOP BY CATEGORIES", 64, 72, 300, 14, BUY_FONT_TEXT, CLR_ORANGE);

    for (int i = 0; i < 8; i++)
        AddButton(g_szCategoryNames[i], 56, 95 + i * 32, 147, 20);

    AddButton("0  CANCEL", 56, 360, 147, 20);
    AddLine(216, 95, 1, 285, 150, 100, 15);
    AddButton("A  AUTOBUY",      229, 95,  160, 20);
    AddButton("R  BUY PREVIOUS", 229, 127, 160, 20);

    UpdateMenu();
}

CBuyMenu::~CBuyMenu() {}

void CBuyMenu::Open()
{
    if (m_pCategory)
        m_pCategory->setVisible(false);

    Reposition();
    UpdateMenu();
    setVisible(true);

    if (gViewPort)
        gViewPort->UpdateCursorState();
}

bool CBuyMenu::IsActive()
{
    return isVisible() || (m_pCategory && m_pCategory->isVisible());
}

void CBuyMenu::Close()
{
    if (m_pCategory)
        m_pCategory->setVisible(false);

    CBuyPanelBase::Close();
}

void CBuyMenu::UpdateMenu()
{
    char buf[64];
    sprintf(buf, "YOU HAVE $%d", gHUD.m_iAccount);
    m_pLblMoney->SetText(buf);
}

bool CBuyMenu::SlotInput(int slot)
{
    if (m_pCategory && m_pCategory->isVisible())
        return m_pCategory->SlotInput(slot);

    if (slot == 0)
    {
        OnButtonPressed(BTN_CANCEL);
        return true;
    }
    if (slot >= 1 && slot <= 8)
    {
        OnButtonPressed(slot - 1);
        return true;
    }
    return false;
}

void CBuyMenu::OnButtonPressed(int i)
{
    if (i < 8)
    {
        OpenCategory(i);
    }
    else if (i == BTN_CANCEL)
    {
        Close();
    }
    else if (i == BTN_AUTOBUY)
    {
        gEngfuncs.pfnClientCmd("autobuy\n");
        Close();
    }
    else if (i == BTN_REBUY)
    {
        gEngfuncs.pfnClientCmd("rebuy\n");
        Close();
    }
}

void CBuyMenu::OpenCategory(int category)
{
    if (category != 1)
        return;

    if (!m_pCategory)
    {
        m_pCategory = new CBuyCategoryMenu();
        m_pCategory->setParent(getParent());
        m_pCategory->setVisible(false);
    }

    m_pCategory->SetupShotguns();
    m_pCategory->Reposition();
    m_pCategory->setVisible(true);
    setVisible(false);

    if (gViewPort)
        gViewPort->UpdateCursorState();
}

CBuyCategoryMenu::CBuyCategoryMenu()
{
    m_iNumItems = 0;
    m_iShownItem = -1;
    memset(m_Items, 0, sizeof(m_Items));

    BuildFrame("BUY");

    for (int i = 0; i < BUY_MAX_ITEMS; i++)
    {
        CBuyButton *b = AddButton("", 56, 95 + i * 32, 147, 20);
        if (b)
            b->setVisible(false);
    }

    AddButton("0  CANCEL", 56, 360, 147, 20);

    m_pImageBox = new Panel(BUY_S(236), BUY_S(95), BUY_S(300), BUY_S(80));
    m_pImageBox->setParent(this);
    m_pImageBox->setPaintBackgroundEnabled(true);
    m_pImageBox->setBgColor(0, 0, 0, 0);
    m_pImageBox->setBorder(m_pBorder[BTN_NORMAL]);

    m_pImageLabel = new Label("", 0, 0, BUY_S(300), BUY_S(80));
    m_pImageLabel->setParent(m_pImageBox);
    m_pImageLabel->setPaintBackgroundEnabled(false);
    m_pImageLabel->setContentAlignment(Label::a_center);

    for (int i = 0; i < BUY_STAT_ROWS; i++)
    {
        int y = 187 + i * 16;
        m_pStatName[i]  = AddLabel(g_szStatNames[i], 236, y, 170, 12, BUY_FONT_TEXT, CLR_ORANGE);
        m_pStatValue[i] = AddLabel("", 410, y, 180, 12, BUY_FONT_TEXT, CLR_ORANGE);
    }
}

CBuyCategoryMenu::~CBuyCategoryMenu() {}

void CBuyCategoryMenu::SetTitle(const char *title)
{
    m_pTitle->SetText(title);
}

void CBuyCategoryMenu::ClearItems()
{
    for (int i = 0; i < m_iNumItems; i++)
    {
        m_Items[i].pImage = NULL;
        m_pButtons[i]->setVisible(false);
    }
    m_iNumItems = 0;
    m_iShownItem = -1;
}

int CBuyCategoryMenu::AddItem(const char *name, int price, const char *command,
                              const char *image, const char *const *stats)
{
    if (m_iNumItems >= BUY_MAX_ITEMS)
        return -1;

    int idx = m_iNumItems++;
    BuyItem_t &it = m_Items[idx];
    memset(&it, 0, sizeof(it));

    strncpy(it.name, name, sizeof(it.name) - 1);
    strncpy(it.command, command, sizeof(it.command) - 1);
    strncpy(it.image, image ? image : "", sizeof(it.image) - 1);
    it.price = price;

    sprintf(it.stats[0], "$%d", price);
    for (int i = 0; i < BUY_STAT_ROWS - 1; i++)
    {
        if (stats && stats[i])
            strncpy(it.stats[i + 1], stats[i], sizeof(it.stats[i + 1]) - 1);
    }

    char buf[96];
    sprintf(buf, "%d  %s", idx + 1, it.name);
    m_pButtons[idx]->SetCaption(buf);
    m_pButtons[idx]->setVisible(true);

    return idx;
}

void CBuyCategoryMenu::ShowItemInfo(int index)
{
    if (index < 0 || index >= m_iNumItems)
        return;

    m_iShownItem = index;
    BuyItem_t &it = m_Items[index];

    if (!it.pImage)
        it.pImage = BuyMenu_LoadTGA(it.image);
    m_pImageLabel->setImage(it.pImage);

    char buf[64];
    for (int i = 0; i < BUY_STAT_ROWS; i++)
    {
        sprintf(buf, ": %s", it.stats[i]);
        m_pStatValue[i]->SetText(buf);
    }
}

void CBuyCategoryMenu::UpdateMenu()
{
    for (int i = 0; i < m_iNumItems; i++)
        SetButtonEnabled(i, gHUD.m_iAccount >= m_Items[i].price);

    SetButtonEnabled(BTN_CANCEL, true);

    if (m_iNumItems > 0)
        ShowItemInfo(m_iShownItem >= 0 ? m_iShownItem : 0);
}

bool CBuyCategoryMenu::SlotInput(int slot)
{
    if (slot == 0)
    {
        OnButtonPressed(BTN_CANCEL);
        return true;
    }
    if (slot >= 1 && slot <= m_iNumItems)
    {
        OnButtonPressed(slot - 1);
        return true;
    }
    return false;
}

void CBuyCategoryMenu::OnButtonHover(int i)
{
    if (i < m_iNumItems)
        ShowItemInfo(i);
}

void CBuyCategoryMenu::OnButtonPressed(int i)
{
    if (i == BTN_CANCEL)
    {
        Close();
        return;
    }

    if (i < 0 || i >= m_iNumItems)
        return;

    if (gHUD.m_iAccount < m_Items[i].price)
        return;

    char cmd[48];
    sprintf(cmd, "%s\n", m_Items[i].command);
    gEngfuncs.pfnClientCmd(cmd);
    Close();
}

void CBuyCategoryMenu::SetupShotguns()
{
    ClearItems();
    SetTitle("BUY SHOTGUNS (PRIMARY WEAPON)");

    const char *m3[8] = { "ITALY", "12 GAUGE", "8 CARTRIDGES", "N/A", "3.5KG", "3.8G", "1250 FT/SEC", "2429J" };
    const char *xm[8] = { "ITALY", "12 GAUGE", "7 CARTRIDGES", "N/A", "3.1KG", "3.8G", "1250 FT/SEC", "2429J" };

    AddItem("LEONE 12 GAUGE SUPER", 1700, "m3",     "gfx/vgui/m3.tga",     m3);
    AddItem("LEONE YG1265 AUTO",    3000, "xm1014", "gfx/vgui/xm1014.tga", xm);

    UpdateMenu();
}
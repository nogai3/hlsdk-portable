#ifndef HLSDK_PORTABLE_VGUI_BUYMENU_H
#define HLSDK_PORTABLE_VGUI_BUYMENU_H

#include <VGUI_Panel.h>
#include <VGUI_Button.h>
#include <VGUI_Label.h>
#include <VGUI_ActionSignal.h>
#include <VGUI_InputSignal.h>
#include <VGUI_LineBorder.h>
#include <VGUI_BitmapTGA.h>

#define BUY_MAX_BUTTONS   16
#define BUY_MAX_ITEMS     9
#define BUY_STAT_ROWS     9

enum
{
    BTN_NORMAL = 0,
    BTN_HOVER,
    BTN_DISABLED
};

class CBuyCategoryMenu;

class CBuyLabel : public vgui::Panel
{
public:
    CBuyLabel(const char *text, int x, int y, int w, int h, vgui::Font *font, int r, int g, int b);

    void SetText(const char *text);
    void SetTextColor(int r, int g, int b);

protected:
    virtual void paint();

private:
    char        m_szText[128];
    vgui::Font *m_pFont;
    int         m_iR, m_iG, m_iB;
};

class CBuyButton : public vgui::Button
{
public:
    CBuyButton(const char *text, int x, int y, int w, int h, vgui::Font *font);

    void SetCaption(const char *text);
    void SetTextColor(int r, int g, int b);

protected:
    virtual void paint();
    virtual void paintBackground();

private:
    char        m_szText[128];
    vgui::Font *m_pFont;
    int         m_iR, m_iG, m_iB;
    int         m_iInset;
};

class CBuyPanelBase : public vgui::Panel, public vgui::ActionSignal, public vgui::InputSignal
{
public:
    CBuyPanelBase();
    virtual ~CBuyPanelBase() {}

    virtual void actionPerformed(vgui::Panel *panel);

    virtual void cursorEntered(vgui::Panel *panel);
    virtual void cursorExited(vgui::Panel *panel);
    virtual void cursorMoved(int x, int y, vgui::Panel *panel) {}
    virtual void mousePressed(vgui::MouseCode code, vgui::Panel *panel) {}
    virtual void mouseDoublePressed(vgui::MouseCode code, vgui::Panel *panel) {}
    virtual void mouseReleased(vgui::MouseCode code, vgui::Panel *panel) {}
    virtual void mouseWheeled(int delta, vgui::Panel *panel) {}
    virtual void keyPressed(vgui::KeyCode code, vgui::Panel *panel) {}
    virtual void keyTyped(vgui::KeyCode code, vgui::Panel *panel) {}
    virtual void keyReleased(vgui::KeyCode code, vgui::Panel *panel) {}
    virtual void keyFocusTicked(vgui::Panel *panel) {}

    virtual bool SlotInput(int slot) { return false; }
    virtual void UpdateMenu() {}
    virtual void Close();

    void Reposition();

protected:
    virtual void OnButtonPressed(int i) {}
    virtual void OnButtonHover(int i) {}

    void BuildFrame(const char *title);

    CBuyButton  *AddButton(const char *text, int x, int y, int w, int h);
    CBuyLabel   *AddLabel(const char *text, int x, int y, int w, int h,
                          const char *fontScheme, int r, int g, int b);
    vgui::Panel *AddLine(int x, int y, int w, int h, int r, int g, int b);

    void SetButtonEnabled(int i, bool enabled);
    int  FindButton(vgui::Panel *panel);
    void ApplyStyle(int i, int state);

    CBuyButton *m_pButtons[BUY_MAX_BUTTONS];
    bool        m_bEnabled[BUY_MAX_BUTTONS];
    int         m_iNumButtons;

    vgui::LineBorder *m_pBorder[3];

    vgui::Label *m_pLogo;
    CBuyLabel   *m_pTitle;
};

class CBuyMenu : public CBuyPanelBase
{
public:
    CBuyMenu();
    virtual ~CBuyMenu();

    void Open();
    bool IsActive();

    virtual void Close();
    virtual void UpdateMenu();
    virtual bool SlotInput(int slot);

protected:
    virtual void OnButtonPressed(int i);

private:
    void OpenCategory(int category);

    enum { BTN_CANCEL = 8, BTN_AUTOBUY = 9, BTN_REBUY = 10 };

    CBuyLabel        *m_pLblMoney;
    CBuyLabel        *m_pLblSubtitle;
    CBuyCategoryMenu *m_pCategory;
};

struct BuyItem_t
{
    char name[48];
    int  price;
    char command[32];
    char image[64];
    char stats[BUY_STAT_ROWS][48];
    vgui::BitmapTGA *pImage;
};

class CBuyCategoryMenu : public CBuyPanelBase
{
public:
    CBuyCategoryMenu();
    virtual ~CBuyCategoryMenu();

    virtual void UpdateMenu();
    virtual bool SlotInput(int slot);

    void SetTitle(const char *title);
    void ClearItems();
    int  AddItem(const char *name, int price, const char *command,
                 const char *image, const char *const *stats);
    void ShowItemInfo(int index);
    void SetupShotguns();

protected:
    virtual void OnButtonPressed(int i);
    virtual void OnButtonHover(int i);

private:
    enum { BTN_CANCEL = BUY_MAX_ITEMS };

    BuyItem_t    m_Items[BUY_MAX_ITEMS];
    int          m_iNumItems;
    int          m_iShownItem;

    vgui::Panel *m_pImageBox;
    vgui::Label *m_pImageLabel;
    CBuyLabel   *m_pStatName[BUY_STAT_ROWS];
    CBuyLabel   *m_pStatValue[BUY_STAT_ROWS];
};

#endif
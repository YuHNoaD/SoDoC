#include <wx/wx.h>
#include "UI.h"

// Lop ung dung chinh cho chuong trinh cay gia dinh
class FamilyTreeApp : public wxApp {
public:
    // Ham khoi tao ung dung
    virtual bool OnInit() {
        // Tao frame chinh cho giao dien
        FamilyTreeFrame* frame = new FamilyTreeFrame();
        // Hien thi frame
        frame->Show(true);
        return true;
    }
};

// Khai bao ung dung wxWidgets
wxIMPLEMENT_APP(FamilyTreeApp);
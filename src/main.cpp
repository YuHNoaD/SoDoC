#include <wx/wx.h>
#include "UI.h"

class FamilyTreeApp : public wxApp {
public:
    virtual bool OnInit() {
        FamilyTree* frame = new FamilyTree();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(FamilyTreeApp);
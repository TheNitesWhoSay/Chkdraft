#include "about.h"
#include <common_files/resource.h>

AboutWindow::AboutWindow()
{

}

AboutWindow::~AboutWindow()
{

}

bool AboutWindow::CreateThis(HWND hParent)
{
    if ( getHandle() == NULL &&
         ClassDialog::CreateModelessDialog(MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hParent) )
    {
        ShowNormal();
        return true;
    }
    else if ( getHandle() != NULL )
        Show();

    return false;
}

bool AboutWindow::DestroyThis()
{
    ClassDialog::Hide();
    return ClassDialog::DestroyDialog();
}

void AboutWindow::CreateSubWindows(HWND hWnd)
{
    WinLib::TextControl licenseText {};
    if ( licenseText.FindThis(hWnd, IDC_STATIC_LICENSE) )
    {
        licenseText.SetText(
            "Copyright (c) 2015-2025 Justin Forsberg\n"
            "\n"
            "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
            "of this software and associated documentation files (the \"Software\"), to deal\n"
            "in the Software without restriction, including without limitation the rights\n"
            "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
            "copies of the Software, and to permit persons to whom the Software is\n"
            "furnished to do so, subject to the following conditions:\n"
            "\n"
            "The above copyright notice and this permission notice shall be included in\n"
            "all copies or substantial portions of the Software.\n"
            "\n"
            "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
            "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
            "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
            "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
            "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
            "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"
            "THE SOFTWARE.\n");
    }

    WinLib::TextControl creditsText {};
    if ( creditsText.FindThis(hWnd, IDC_CREDITS) )
    {
        creditsText.SetText("- Blizzard Entertainment - Creator of StarCraft and the staredit campaign editor."
            " Any graphics, sounds, text, and other resources loaded or included from StarCraft and Staredit are property of Blizzard Entertainment.\n"
            " - SI - Developer of Scmdraft from which Chkdraft draws many ideas, UI designs, and so fourth.\n"
            " - Justin Forsberg - Owner and lead developer of Chkdraft.\n"
            " - SaintOfIdiocy, Adam Heinermann, and Corbo - Direct contributions inside and outside of git, including code, testing, and miscellaneous assistance.\n"
            " - RElesgoe, Zoltan Kovacs, Caden-Fletcher and Milestone - Direct contributions to the project on git.\n"
            " - Sayoka - Created the icon for chkdraft.\n"
            " - 3crow and zzlk - Financial contributions towards development of modding and graphical features."
            " - Lanthanide, Oh_Man, CecilSunkure, Wormer, Roy, and many other members of the SEN community for ideas, testing, and other forms of support.");
    }
}

BOOL AboutWindow::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        case WM_INITDIALOG:
            {
                SetSmallIcon(WinLib::ResourceManager::getIcon(IDI_PROGRAM_ICON, 16, 16));
                CreateSubWindows(hWnd);
                defaultChildFonts();
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;

        case WM_CLOSE:
            ShowWindow(hWnd, SW_HIDE);
            break;

        default:
            return FALSE;
            break;
    }
    return TRUE;
}

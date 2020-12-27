#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif
    #include <windows.h>
    #define MAIN_decl __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
    #define MAIN_decl main(int, char **)
#endif

int MAIN_decl {
    int fake_argc = 1;
    char empty[] = "what";
    char *fake_argv[] = { empty, nullptr };
    QApplication app { fake_argc, fake_argv };
    auto const mainwnd = new QWidget { };
    auto const text = new QLabel { "GAMERS BILL OF RIGHTS" };
    auto const layout = new QVBoxLayout { };
    layout->addWidget(text);
    mainwnd->setLayout(layout);

    auto const btngroup = new QDialogButtonBox { };
    auto const ok = new QPushButton { "ok" };
    auto const no = new QPushButton { "no" };
    btngroup->addButton(ok, QDialogButtonBox::AcceptRole);
    btngroup->addButton(no, QDialogButtonBox::RejectRole);

    layout->addWidget(btngroup);
    mainwnd->show();
    app.exec();
    return 0;
}

#include "../stubs/c++-stubs.cpp"

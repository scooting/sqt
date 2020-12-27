#pragma once
#include "../QtGui/qtgui-config.h"

// -----------------------------------------------------------------------------
// generally useful
// -----------------------------------------------------------------------------
#define QT_FEATURE_combobox 1
#define QT_FEATURE_contextmenu 1
#define QT_FEATURE_dialog 1
#define QT_FEATURE_dialogbuttonbox 1
#define QT_FEATURE_groupbox 1
#define QT_FEATURE_lineedit 1
#define QT_FEATURE_mainwindow 1
#define QT_FEATURE_menu 1
#define QT_FEATURE_menubar 1
#define QT_FEATURE_progressbar 1
#define QT_FEATURE_pushbutton 1
#define QT_FEATURE_scrollbar 1
#define QT_FEATURE_splitter 1
#define QT_FEATURE_statusbar 1
#define QT_FEATURE_statustip 1
#define QT_FEATURE_textedit 1
#define QT_FEATURE_toolbar 1
#define QT_FEATURE_toolbutton 1
#define QT_FEATURE_treeview 1
#define QT_FEATURE_tooltip 1

// -----------------------------------------------------------------------------
// questionable utility
// -----------------------------------------------------------------------------
#define QT_FEATURE_abstractbutton 1
#define QT_FEATURE_abstractslider 1
#define QT_FEATURE_action 1
#define QT_FEATURE_buttongroup 1
#define QT_FEATURE_checkbox 1
#define QT_FEATURE_columnview 1
#define QT_FEATURE_fontcombobox 1
#define QT_FEATURE_formlayout 1
#define QT_FEATURE_itemviews 1
#define QT_FEATURE_label 1
#define QT_FEATURE_listview 1
#define QT_FEATURE_listwidget 1
#define QT_FEATURE_messagebox 1
#define QT_FEATURE_radiobutton 1
#define QT_FEATURE_resizehandler 1
#define QT_FEATURE_rubberband 1
#define QT_FEATURE_scrollarea 1
#define QT_FEATURE_sizegrip 1
#define QT_FEATURE_slider 1
#define QT_FEATURE_spinbox 1
#define QT_FEATURE_stackedwidget 1
#define QT_FEATURE_style_fusion 1
#define QT_FEATURE_tabbar 1
#define QT_FEATURE_tableview 1
#define QT_FEATURE_tablewidget 1
#define QT_FEATURE_treewidget 1
#define QT_FEATURE_tabwidget 1
#define QT_FEATURE_toolbox 1
#define QT_FEATURE_widgettextcontrol 1
#define QT_FEATURE_validator 1
#define QT_FEATURE_opengl 1
#define QT_FEATURE_errormessage 1
#define QT_FEATURE_filedialog 1
#define QT_FEATURE_fontdialog 1
#define QT_FEATURE_filesystemmodel 1

// -----------------------------------------------------------------------------
// useless
// -----------------------------------------------------------------------------
#define QT_FEATURE_calendarwidget -1
#define QT_FEATURE_commandlinkbutton -1
#define QT_FEATURE_completer -1
#define QT_FEATURE_datetimeedit -1
#define QT_FEATURE_dial -1
#define QT_FEATURE_dockwidget -1
#define QT_FEATURE_effects -1
#define QT_FEATURE_graphicseffect -1
#define QT_FEATURE_graphicsview -1
#define QT_FEATURE_keysequenceedit -1
#define QT_FEATURE_mdiarea -1
#define QT_FEATURE_movie -1
#define QT_FEATURE_scroller -1
#define QT_FEATURE_statemachine -1
#define QT_FEATURE_style_windows -1
#define QT_FEATURE_wizard -1
#define QT_FEATURE_lcdnumber -1
#define QT_FEATURE_splashscreen -1
#define QT_FEATURE_textbrowser -1
#define QT_FEATURE_datawidgetmapper -1
#define QT_FEATURE_dirmodel -1
#define QT_FEATURE_undocommand -1
#define QT_FEATURE_undogroup -1
#define QT_FEATURE_undoview -1
#define QT_FEATURE_colordialog -1
#define QT_FEATURE_fscompleter -1
#define QT_FEATURE_inputdialog -1
#define QT_FEATURE_progressdialog -1

#define QT_FEATURE_style_stylesheet -1
#if !defined(QT_NO_STYLE_STYLESHEET)
    #define QT_NO_STYLE_STYLESHEET
#endif

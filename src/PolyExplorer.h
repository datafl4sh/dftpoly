#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "points.h"
#include "PolyDFTWidget.h"

#ifdef HAVE_DISKPP
#include "StabfreeHHOParameters.h"
#include "HHOModel.h"
#endif

class PolyExplorer : public QMainWindow {
public:
    PolyExplorer();

public slots:
    void savePolygon(void);
#ifdef HAVE_DISKPP
    void saveEigenFunctions();
#endif

private:
    void makePolygonMenu();
    void makePolyDFTWidget();
    void makeStabfreeHHOWidget();

    QMenu *viewMenu;
    QMenu *polygonMenu;

    PolygonEditorWidget     *polygonEditorWidget;
    PolyDFTWidget           *polyDFTWidget;

#ifdef HAVE_DISKPP
    HHOModel                *hhoModel;
    StabfreeHHOParamsWidget *stabfreeWidget;
#endif
};
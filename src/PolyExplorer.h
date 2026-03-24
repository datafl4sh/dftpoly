#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "points.h"
#include "PolyDFTWidget.h"
#include "StabfreeHHOParameters.h"

class PolyExplorer : public QMainWindow {
public:
    PolyExplorer();

private:
    void makePolygonMenu();
    void makePolyDFTWidget();
    void makeStabfreeHHOWidget();

    QMenu *viewMenu;
    QMenu *polygonMenu;

    PolygonEditorWidget     *polygonEditorWidget;
    PolyDFTWidget           *polyDFTWidget;
    StabfreeHHOParamsWidget *stabfreeWidget;
};
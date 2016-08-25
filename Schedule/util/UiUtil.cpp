#include "UiUtil.h"
#include "util/Util.h"
#include "util/ConfigUtil.h"
#include "bean/ScheduleDescription.h"
#include "bean/ScheduleItem.h"
#include "Constants.h"
#include "gui/ScheduleItemWidget.h"

#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QApplication>

#include <QLabel>
#include <QWidget>
#include <QStackedWidget>
#include <QSpacerItem>
#include <QGridLayout>
#include <QTableView>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QModelIndex>

void UiUtil::loadQss() {
    QStringList qssFileNames = Singleton<ConfigUtil>::getInstance().getQssFiles();
    QString qss;

    foreach (QString name, qssFileNames) {
        qDebug() << QString("=> 加载 QSS 文件 %1").arg(name);

        QFile file(name);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << QString("打开 QSS 文件 %1 失败");
            continue;
        }

        qss.append(file.readAll()).append("\n");
        file.close();
    }

    if (!qss.isEmpty()) {
        //        qDebug() << qss;
        qApp->setStyleSheet(qss);
    }
}

void UiUtil::updateQss(QWidget *widget) {
    widget->setStyleSheet("/**/");
}

/**
 * 把 widget 加入到 stacked widget 里, 可以设置向四个方向的伸展
 */
void UiUtil::addWidgetIntoStackedWidget(QWidget *widget, QStackedWidget *stackedWidget,
                                        bool toLeft,
                                        bool toTop,
                                        bool toRight,
                                        bool toBottom) {
    // 使用 widget 居左上
    QGridLayout *layout = new QGridLayout();

    QSpacerItem *spacer = NULL;
    if (!toLeft) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 1, 0);
    }

    if (!toTop) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, 0, 1);
    }

    if (!toRight) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addItem(spacer, 1, 2);
    }

    if (!toBottom) {
        spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer, 2, 1);
    }

    QWidget *container = new QWidget();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget, 1, 1);
    container->setLayout(layout);
    stackedWidget->addWidget(container);
}

void UiUtil::setWidgetPaddingAndSpacing(QWidget *widget, int padding, int spacing) {
    // 设置 Widget 的 padding 和 spacing
    QLayout *layout = widget->layout();

    if (NULL != layout) {
        layout->setContentsMargins(padding, padding, padding, padding);
        layout->setSpacing(spacing);
    }
}

QModelIndex UiUtil::getTableViewSelectedIndex(QTableView *view) {
    QItemSelectionModel *ism = view->selectionModel();
    QModelIndexList mil = ism->selectedIndexes();

    return mil.count() > 0 ? mil.at(0) : QModelIndex();
}

void UiUtil::appendTableViewRow(QTableView *view, int editColumn) {
    QAbstractItemModel *model = view->model();
    int row = model->rowCount();
    model->insertRow(row);

    QModelIndex index = model->index(row, editColumn);
    if (!index.isValid()) { return; }

    view->setCurrentIndex(index);
    view->edit(index);
}

void UiUtil::removeTableViewSelectedRow(QTableView *view) {
    QModelIndex index = getTableViewSelectedIndex(view);

    if (index.isValid()) {
        view->model()->removeRow(index.row());
    }
}

void UiUtil::showSchedule(QWidget *scheduleWidget,
                          const ScheduleDescription &desc,
                          const QList<ScheduleItem> &items,
                          bool classVisible,
                          bool teacherVisible,
                          int classId) {
    // 1. 删除 scheduleWidget 中的数据
    // 2. 显示星期
    // 3. 显示课程时间
    // 4. 显示课程信息

    // [1] 删除课程表中已有的 widgets.
    deleteChildrenAndLayout(scheduleWidget);

    // 设置 padding and spacing 为 1，模拟 layout 中的 widget 的边框为 1 个像素
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(1, 1, 1, 1);
    gridLayout->setSpacing(1);

    // [2] 显示星期
    for (int day = 1; day <= desc.dayCountOfWeek; ++day) {
        QLabel *label = new QLabel(desc.dayTexts.value(day));
        label->setAlignment(Qt::AlignCenter);
        label->setProperty(Constants::QSS_CLASS, Constants::QSS_CLASS_DAY_LABEL);
        label->setProperty(Constants::PROPERTY_ROW, 0); // 存储在 layout 中的位置
        label->setProperty(Constants::PROPERTY_COLUMN, day);
        gridLayout->addWidget(label, 0, day);
    }

    // [3] 显示课程时间
    for (int course = 0; course <= desc.courseCountOfDay; ++course) {
        QLabel *label = new QLabel(desc.timeTexts.value(course));
        label->setAlignment(Qt::AlignCenter);
        label->setProperty(Constants::QSS_CLASS, Constants::QSS_CLASS_TIME_LABEL);
        label->setProperty(Constants::PROPERTY_ROW, course); // 存储在 layout 中的位置
        label->setProperty(Constants::PROPERTY_COLUMN, 0);
        gridLayout->addWidget(label, course, 0);
    }

    // [4] 显示课程信息: 先把课表排满，不存在的课用空的 ScheduleItem 表示，然后再显示出来
    QList<ScheduleItem> finalItems = Util::fullScheduleItemsForClass(items, desc, classId);

    foreach (const ScheduleItem &item, finalItems) {
        ScheduleItemWidget *w = new ScheduleItemWidget(item, classVisible, teacherVisible);
        gridLayout->addWidget(w, item.time, item.day);
    }

    scheduleWidget->setLayout(gridLayout);
}

void UiUtil::deleteChildrenAndLayout(QWidget *w) {
    QLayout *layout = w->layout();

    if (layout != NULL) {
        QLayoutItem *child;

        while ((child = layout->itemAt(0)) != NULL) {
            delete child->widget();
        }

        delete layout;
    }
}

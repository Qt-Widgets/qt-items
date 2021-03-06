/*
   Copyright (c) 2008-1015 Alex Zhondin <qtinuum.team@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef QI_SORTING_H
#define QI_SORTING_H

#include "core/Model.h"
#include "core/Range.h"
#include "core/ext/ViewModeled.h"
#include "core/ext/ControllerMouseCaptured.h"
#include "space/grid/GridID.h"

namespace Qi
{

class ModelComparable;
class Range;
class SpaceGrid;

class QI_EXPORT ModelGridSortingBase: public Model
{
    Q_OBJECT
    Q_DISABLE_COPY(ModelGridSortingBase)

public:
    ModelGridSortingBase(QSharedPointer<SpaceGrid> grid);

    QSharedPointer<ModelComparable> sortingModel(GridID id) const { return sortingModelImpl(id); }

    GridID activeSortingId() const { return !m_sortingExpired ? m_activeSortingId : GridID(); }
    void clearActiveSortingId();
    bool isAscending() const { return m_ascending; }
    void setSorting(GridID id, bool ascending);

    bool sort();
    bool sortByItem(GridID id);
    bool defaultSortByItem(GridID id);
    bool sortByItem(GridID id, bool ascending);

signals:
    void willSortItems(const ModelGridSortingBase*);
    void didSortItems(const ModelGridSortingBase*);

protected:
    virtual QSharedPointer<ModelComparable> sortingModelImpl(GridID /*id*/) const = 0;

    void connectModel(const Model* model);
    void disconnectModel(const Model* model);

private:
    void onSortingModelChanged(const Model* model);

    QSharedPointer<SpaceGrid> m_grid;
    bool m_ascending;
    GridID m_activeSortingId;
    bool m_sortingExpired;
};

class QI_EXPORT ModelGridSorting: public ModelGridSortingBase
{
    Q_OBJECT
    Q_DISABLE_COPY(ModelGridSorting)

public:
    ModelGridSorting(QSharedPointer<SpaceGrid> grid);
    virtual ~ModelGridSorting();

    void addSortingModel(GridID id, const QSharedPointer<ModelComparable>& model);
    void addSortingModel(int column, const QSharedPointer<ModelComparable>& model);
    void clear();

protected:
    QSharedPointer<ModelComparable> sortingModelImpl(GridID id) const override;

private:
    QMap<GridID, QSharedPointer<ModelComparable>> m_modelsToSort;
};

class QI_EXPORT ModelGridSortingByRanges: public ModelGridSortingBase
{
    Q_OBJECT
    Q_DISABLE_COPY(ModelGridSortingByRanges)

public:
    ModelGridSortingByRanges(QSharedPointer<SpaceGrid> grid);
    virtual ~ModelGridSortingByRanges();

    void addSortingModel(const QSharedPointer<ModelComparable>& model, const QSharedPointer<Range>& range);
    void clear();

protected:
    QSharedPointer<ModelComparable> sortingModelImpl(GridID id) const override;

private:
    struct SortingInfo
    {
        QSharedPointer<Range> range;
        QSharedPointer<ModelComparable> model;
    };
    QVector<SortingInfo> m_modelsToSort;
};

class QI_EXPORT SortingHub
{
public:
    SortingHub();
    SortingHub(const QSharedPointer<ModelGridSortingBase>& sorting1, const QSharedPointer<ModelGridSortingBase>& sorting2);
    ~SortingHub();

    void addSorting(const QSharedPointer<ModelGridSortingBase>& sorting);

    void clearActiveSortingId();
    void sort();

private:
    void onDidSortItems(const ModelGridSortingBase* activeSorting);

    struct Info
    {
        QSharedPointer<ModelGridSortingBase> sorting;
        QMetaObject::Connection connection;
    };

    QVector<Info> m_sortings;
};

class QI_EXPORT RangeGridSorting: public Range
{
    Q_OBJECT
    Q_DISABLE_COPY(RangeGridSorting)

public:
    RangeGridSorting(const QSharedPointer<ModelGridSortingBase>& model, int row = 0);

protected:
    bool hasItemImpl(ID id) const override;

private:
    QSharedPointer<ModelGridSortingBase> m_model;
    int m_row;
};

inline QSharedPointer<RangeGridSorting> makeRangeGridSorter(const QSharedPointer<ModelGridSortingBase>& model, int row = 0)
{
    return QSharedPointer<RangeGridSorting>::create(model, row);
}

class QI_EXPORT ViewGridSorting: public ViewModeled<ModelGridSortingBase>
{
    Q_OBJECT
    Q_DISABLE_COPY(ViewGridSorting)

public:
    ViewGridSorting(const QSharedPointer<ModelGridSortingBase>& model, bool useDefaultController = true);

protected:
    void drawImpl(QPainter* painter, const GuiContext& ctx, const CacheContext& cache, bool* showTooltip) const override;
    bool tooltipTextImpl(ID id, QString& txt) const override;
};

class QI_EXPORT ControllerMouseGridSorting: public ControllerMouseCaptured
{
    Q_OBJECT
    Q_DISABLE_COPY(ControllerMouseGridSorting)

public:
    ControllerMouseGridSorting(const QSharedPointer<ModelGridSortingBase>& model);

protected:
    void applyImpl() override;

private:
    QSharedPointer<ModelGridSortingBase> m_model;
};

} // end namespace Qi

#endif // QI_SORTING_H

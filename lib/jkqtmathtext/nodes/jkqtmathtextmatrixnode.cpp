/*
    Copyright (c) 2008-2022 Jan W. Krieger (<jan@jkrieger.de>)

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "jkqtmathtext/nodes/jkqtmathtextmatrixnode.h"
#include "jkqtmathtext/jkqtmathtexttools.h"
#include "jkqtmathtext/jkqtmathtext.h"
#include "jkqtcommon/jkqtpcodestructuring.h"
#include "jkqtcommon/jkqtpstringtools.h"
#include <cmath>
#include <QFontMetricsF>
#include <QDebug>
#include <QFontDatabase>
#include <QFontInfo>
#include <QApplication>
#include <QFont>







JKQTMathTextMatrixNode::JKQTMathTextMatrixNode(JKQTMathText* _parent, const QVector<QVector<JKQTMathTextNode *> > &children, const QString &columnSpec):
    JKQTMathTextNode(_parent),
    verticalLineRHSColumn(),
    verticalLineLeft(LTnone),
    horizontalLineBottomRow(),
    horizontalLineTop(LTnone),
    columns(0),
    lines(0)
{
    setChildren(children);
    parseColumnSpec(columnSpec);
}

JKQTMathTextMatrixNode::JKQTMathTextMatrixNode(JKQTMathText *_parent, const QString &columnSpec):
    JKQTMathTextNode(_parent),
    verticalLineRHSColumn(),
    verticalLineLeft(LTnone),
    horizontalLineBottomRow(),
    horizontalLineTop(LTnone),
    columns(0),
    lines(0)
{
    parseColumnSpec(columnSpec);
}

JKQTMathTextMatrixNode::~JKQTMathTextMatrixNode() {
    for (int i=0; i<children.size(); i++) {
        for (int j=0; j<children[i].size(); j++) {
            delete children[i].at(j);
        }
    }
    children.clear();
}

void JKQTMathTextMatrixNode::setChildren(const QVector<QVector<JKQTMathTextNode *> > &children)
{
    this->lines=children.size();
    this->columns=0;
    for (int i=0; i<children.size(); i++) {
        if (children[i].size()>this->columns) this->columns=children[i].size();
    }
    this->children=children;
    for (int i=0; i<children.size(); i++) {
        for (int j=0; j<children[i].size(); j++) {
            children[i].operator[](j)->setParentNode(this);
        }
    }
}

void JKQTMathTextMatrixNode::setRowBottomLine(int col, LineType line)
{
    horizontalLineBottomRow[col]=line;
}

void JKQTMathTextMatrixNode::setTopLine(LineType line)
{
    horizontalLineTop=line;
}

QString JKQTMathTextMatrixNode::getTypeName() const
{
    return "MTmatrixNode";
}


bool JKQTMathTextMatrixNode::toHtml(QString &/*html*/, JKQTMathTextEnvironment /*currentEv*/, JKQTMathTextEnvironment /*defaultEv*/)
{
    return false;
}

QVector<QVector<JKQTMathTextNode *> > JKQTMathTextMatrixNode::getChildren() const {
    return this->children;
}

int JKQTMathTextMatrixNode::getColumns() const {
    return this->columns;
}

int JKQTMathTextMatrixNode::getLines() const {
    return this->lines;
}


void JKQTMathTextMatrixNode::setDrawBoxes(bool draw)
{
    this->drawBoxes=draw;
    for (int i=0; i<lines; i++) {
        for (int j=0; j<children[i].size(); j++) {
            children[i].at(j)->setDrawBoxes(draw);
        }
    }

}

void JKQTMathTextMatrixNode::drawVLine(QPainter &painter, double x, double y, double height, LineType lt, double linewidth, double linewidthHeavy, QColor color, double doublelineseparation)
{
    if (lt==LTnone) return;
    QPen p(color, linewidth, Qt::SolidLine);
    if (lt==LTdashed || lt==LTdoubleDashed) p.setStyle(Qt::DashLine);
    if (lt==LTheavyline) p.setWidth(linewidthHeavy);
    painter.save(); auto __finalpaint=JKQTPFinally([&painter]() {painter.restore();});
    painter.setPen(p);
    if (lt==LTline || lt==LTdashed || lt==LTheavyline) {
        painter.drawLine(QPointF(x,y), QPointF(x,y+height));
    } else if (lt==LTdoubleline || lt==LTdoubleDashed) {
        painter.drawLine(QPointF(x-doublelineseparation/2.0,y), QPointF(x-doublelineseparation/2.0,y+height));
        painter.drawLine(QPointF(x+doublelineseparation/2.0,y), QPointF(x+doublelineseparation/2.0,y+height));
    }
}

void JKQTMathTextMatrixNode::drawHLine(QPainter &painter, double x, double y, double width, LineType lt, double linewidth, double linewidthHeavy, QColor color, double doublelineseparation)
{
    if (lt==LTnone) return;
    QPen p(color, linewidth, Qt::SolidLine);
    if (lt==LTdashed || lt==LTdoubleDashed) p.setStyle(Qt::DashLine);
    if (lt==LTheavyline) p.setWidth(linewidthHeavy);
    painter.save(); auto __finalpaint=JKQTPFinally([&painter]() {painter.restore();});
    painter.setPen(p);
    if (lt==LTline || lt==LTdashed || lt==LTheavyline) {
        painter.drawLine(QPointF(x,y), QPointF(x+width,y));
    } else if (lt==LTdoubleline || lt==LTdoubleDashed) {
        painter.drawLine(QPointF(x,y-doublelineseparation/2.0), QPointF(x+width,y-doublelineseparation/2.0));
        painter.drawLine(QPointF(x,y+doublelineseparation/2.0), QPointF(x+width,y+doublelineseparation/2.0));
    }
}

void JKQTMathTextMatrixNode::parseColumnSpec(const QString &columnSpec)
{
    int i=0;
    while (i<columnSpec.size()) {
        if (columnSpec[i]=='l') {
            columnAlignment<<MTHALeft;
        } else if (columnSpec[i]=='c') {
            columnAlignment<<MTHACentered;
        } else if (columnSpec[i]=='r') {
            columnAlignment<<MTHARight;
        } else if (columnSpec.mid(i,2)=="||") {
            if (columnAlignment.size()==0) {
                verticalLineLeft=LTdoubleline;
            } else {
                verticalLineRHSColumn[columnAlignment.size()-1]=LTdoubleline;
            }
            i++;
        } else if (columnSpec.mid(i,2)=="::") {
            if (columnAlignment.size()==0) {
                verticalLineLeft=LTdoubleDashed;
            } else {
                verticalLineRHSColumn[columnAlignment.size()-1]=LTdoubleDashed;
            }
            i++;
        } else if (columnSpec.mid(i,1)=="|") {
            if (columnAlignment.size()==0) {
                verticalLineLeft=LTline;
            } else {
                verticalLineRHSColumn[columnAlignment.size()-1]=LTline;
            }
        } else if (columnSpec.mid(i,1)==":") {
            if (columnAlignment.size()==0) {
                verticalLineLeft=LTdashed;
            } else {
                verticalLineRHSColumn[columnAlignment.size()-1]=LTdashed;
            }
        } else if (!columnSpec[i].isSpace()){
            parentMathText->addToErrorList(QString("array spec has unknown character '%1' (full spec was '%2')").arg(columnSpec[i]).arg(columnSpec));
        }
        i++;
    }
}




JKQTMathTextMatrixNode::LayoutInfo::LayoutInfo():
    JKQTMathTextNodeSize(), colwidth(), rowheight()
{

}

JKQTMathTextMatrixNode::LayoutInfo JKQTMathTextMatrixNode::calcLayout(QPainter &painter, const JKQTMathTextEnvironment &currentEv) const
{
    LayoutInfo l;

    const QFontMetricsF fm(currentEv.getFont(parentMathText), painter.device());
    JKQTMathTextEnvironment ev1=currentEv;

    const double xheight=fm.strikeOutPos();
    const double xwidth=fm.boundingRect("x").width();
    const double XPadding=parentMathText->getMatrixXPaddingFactor()*xwidth;
    const double YPadding=parentMathText->getMatrixYPaddingFactor()*xwidth;
    const double XSeparation=parentMathText->getMatrixXSeparationFactor()*xwidth;
    const double YSeparation=parentMathText->getMatrixYSeparationFactor()*xwidth;

    l.colwidth.resize(columns); for (int i=0; i<columns; i++) l.colwidth[i]=0;
    l.rowheight.resize(lines);
    l.rowascent.resize(lines);
    l.cellwidth.resize(lines);
    l.leftPadding=(verticalLineLeft==LTnone)?0:XPadding;
    l.rightPadding=(verticalLineRHSColumn.value(columns-1,LTnone)==LTnone)?0:XPadding;
    l.topPadding=(horizontalLineTop==LTnone)?0:YPadding;
    l.bottomPadding=(horizontalLineBottomRow.value(lines-1,LTnone)==LTnone)?0:YPadding;
    for (int ll=0; ll<lines; ll++) l.cellwidth[ll]=QVector<double>(columns, 0.0);
    QVector<double> rowdescent;
    rowdescent.resize(lines);
    for (int i=0; i<lines; i++) {
        l.rowheight[i]=0;
        l.rowascent[i]=0;
        rowdescent[i]=0;
        for (int j=0; j<children[i].size(); j++) {
            const JKQTMathTextNodeSize cs=children[i].at(j)->getSize(painter, ev1);
            l.cellwidth[i].operator[](j)=cs.width;
            l.colwidth[j]=qMax(l.colwidth[j], cs.width);
            l.rowascent[i]=qMax(l.rowascent[i], cs.baselineHeight);
            rowdescent[i]=qMax(rowdescent[i], cs.getDescent());
        }
        l.rowheight[i]=l.rowascent[i]+rowdescent[i];
    }


    l.overallHeight=(lines-1)*YSeparation+l.topPadding+l.bottomPadding;
    l.width=(columns-1)*XSeparation+l.leftPadding+l.rightPadding;
    for (int i=0; i<columns; i++) l.width=l.width+l.colwidth[i];
    for (int i=0; i<lines; i++) l.overallHeight=l.overallHeight+l.rowheight[i];
    l.baselineHeight=l.overallHeight/2.0+xheight;
    l.strikeoutPos=xheight;

    return l;
}

void JKQTMathTextMatrixNode::getSizeInternal(QPainter& painter, JKQTMathTextEnvironment currentEv, double& width, double& baselineHeight, double& overallHeight, double& strikeoutPos, const JKQTMathTextNodeSize* /*prevNodeSize*/) {
    const  LayoutInfo l=calcLayout(painter, currentEv);
    width=l.width;
    overallHeight=l.overallHeight;
    baselineHeight=l.baselineHeight;
    strikeoutPos=l.strikeoutPos;
}

double JKQTMathTextMatrixNode::draw(QPainter& painter, double x, double y, JKQTMathTextEnvironment currentEv, const JKQTMathTextNodeSize* /*prevNodeSize*/) {
    doDrawBoxes(painter, x, y, currentEv);

    const QFontMetricsF fm(currentEv.getFont(parentMathText), painter.device());
    JKQTMathTextEnvironment ev1=currentEv;

    const  LayoutInfo l=calcLayout(painter, currentEv);

    const double xheight=fm.strikeOutPos();
    const double xwidth=fm.boundingRect("x").width();
    //const double XPadding=parentMathText->getMatrixXPaddingFactor()*xwidth;
    //const double YPadding=parentMathText->getMatrixYPaddingFactor()*xwidth;
    const double XSeparation=parentMathText->getMatrixXSeparationFactor()*xwidth;
    const double YSeparation=parentMathText->getMatrixYSeparationFactor()*xwidth;
    const double yTop=y-l.baselineHeight+l.topPadding;
    const double xLeft=x+l.leftPadding;
    const double linewidth=parentMathText->getMatrixLinewidthThinFactor()*fm.lineWidth();
    const double linewidthThick=parentMathText->getMatrixLinewidthHeavyFactor()*fm.lineWidth();
    const double lineSeparation=parentMathText->getMatrixLineSeparationFactor()*fm.lineWidth();
    double leftlineX=xLeft-l.leftPadding/2.0;
    double rightlineX=x+l.width-l.rightPadding/2.0;
    double toplineY=yTop-l.topPadding/2.0;
    double bottomlineY=y+l.getDescent()-l.bottomPadding/2;
    if (verticalLineLeft==LTdoubleline || verticalLineLeft==LTdoubleDashed) leftlineX-=linewidth;
    if (horizontalLineTop==LTdoubleline || horizontalLineTop==LTdoubleDashed) toplineY-=linewidth;
    if (verticalLineRHSColumn.value(lines-1, LTnone)==LTdoubleline || verticalLineRHSColumn.value(lines-1, LTnone)==LTdoubleDashed) rightlineX+=linewidth;
    if (horizontalLineBottomRow.value(columns-1, LTnone)==LTdoubleline || horizontalLineBottomRow.value(columns-1, LTnone)==LTdoubleDashed) bottomlineY+=linewidth;



    double xx;
    double yy=yTop;
    if (lines>0) yy=yy+l.rowascent[0];
    for (int i=0; i<lines; i++) {
        xx=xLeft;
        for (int j=0; j<children[i].size(); j++) {
            const double cw=l.getCellwidth(i, j);
            double xoffset=0;
            if (columnAlignment.value(j, MTHALeft)==MTHACentered) xoffset=(l.colwidth[j]-cw)/2.0;
            else if (columnAlignment.value(j, MTHALeft)==MTHARight) xoffset=(l.colwidth[j]-cw);
            children[i].at(j)->draw(painter, xx+xoffset, yy, ev1);
            xx=xx+l.colwidth[j]+XSeparation;
            if (i==0 && j<columns-1) drawVLine(painter, xx-XSeparation/2.0, toplineY, bottomlineY-toplineY, verticalLineRHSColumn.value(j, LTnone), linewidth, linewidthThick, currentEv.color, lineSeparation);
        }

        if (i<lines-1) {
            double yline=yy+(l.rowheight[i]-l.rowascent[i])+YSeparation/2.0;
            yy=yline+YSeparation/2.0+l.rowascent[i+1];
            drawHLine(painter, leftlineX, yline, rightlineX-leftlineX, horizontalLineBottomRow.value(i, LTnone), linewidth, linewidthThick, currentEv.color, lineSeparation);
        }
    }

    drawVLine(painter, leftlineX, toplineY, bottomlineY-toplineY, verticalLineLeft, linewidth, linewidthThick, currentEv.color, lineSeparation);
    drawVLine(painter, rightlineX, toplineY, bottomlineY-toplineY, verticalLineRHSColumn.value(columns-1, LTnone), linewidth, linewidthThick, currentEv.color, lineSeparation);
    drawHLine(painter, leftlineX, toplineY, rightlineX-leftlineX, verticalLineLeft, linewidth, linewidthThick, currentEv.color, lineSeparation);
    drawHLine(painter, leftlineX, bottomlineY, rightlineX-leftlineX, horizontalLineBottomRow.value(lines-1, LTnone), linewidth, linewidthThick, currentEv.color, lineSeparation);

    return x+l.width;
}

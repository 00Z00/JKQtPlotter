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



#include "jkqtmathtext/nodes/jkqtmathtextnodetools.h"
#include "jkqtmathtext/nodes/jkqtmathtextlistnode.h"
#include "jkqtmathtext/nodes/jkqtmathtextwhitespacenode.h"
#include "jkqtmathtext/nodes/jkqtmathtexttextnode.h"
#include "jkqtmathtext/jkqtmathtexttools.h"
#include "jkqtmathtext/jkqtmathtext.h"
#include <QDebug>

JKQTMathTextNode *simplifyJKQTMathTextNode(JKQTMathTextNode *node)
{
    JKQTMathTextHorizontalListNode* nl=dynamic_cast<JKQTMathTextHorizontalListNode*>(node);
    JKQTMathTextMultiChildNode* nmc=dynamic_cast<JKQTMathTextMultiChildNode*>(node);
    JKQTMathTextSingleChildNode* nsc=dynamic_cast<JKQTMathTextSingleChildNode*>(node);
    if (nl) {
        if (nl->childCount()==1) {
            // if there was only a single node: simplify the syntax tree, by removing the outer list node
            JKQTMathTextNode* ret= simplifyJKQTMathTextNode(nl->getChild(0));
            nl->clearChildren(false);
            ret->setParentNode(nullptr);
            delete nl;
            return ret;
        } else if (nl->childCount()>1) {
            // if there are several child nodes, apply this method recursively
            for (int i=0; i<nl->childCount(); i++) {
                JKQTMathTextNode* c=nl->getChild(i);
                JKQTMathTextNode* newc= simplifyJKQTMathTextNode(c);
                if (c!=newc) nl->replaceAndDeleteChild(i, newc);
            }
            return nl;
        }
        return nl;
    } else if (nmc) {
        // apply this method recursively to any children
        for (int i=0; i<nmc->childCount(); i++) {
            JKQTMathTextNode* c=nmc->getChild(i);
            JKQTMathTextNode* newc= simplifyJKQTMathTextNode(c);
            if (c!=newc) nmc->replaceAndDeleteChild(i, newc);
        }
        return nmc;
    } else if (nsc) {
        // apply this method recursively to the child
        JKQTMathTextNode* c=nsc->getChild();
        JKQTMathTextNode* newc= simplifyJKQTMathTextNode(c);
        if (c!=newc) nsc->replaceAndDeleteChild(newc);
        return nsc;
    }
    return node;
}

JKQTMathTextNode *simplifyAndTrimJKQTMathTextNode(JKQTMathTextNode *node)
{
    auto cleanText=[](JKQTMathTextNode* t) {
        JKQTMathTextMultiChildNode* nmc=dynamic_cast<JKQTMathTextMultiChildNode*>(t);
        JKQTMathTextNode* tt0=(nmc&&nmc->childCount()>0)?nmc->getFirstChild():t;
        JKQTMathTextNode* tt1=(nmc&&nmc->childCount()>0)?nmc->getLastChild():t;
        JKQTMathTextTextNode* txt0=dynamic_cast<JKQTMathTextTextNode*>(tt0);
        if (txt0) {
            txt0->removeLeadingWhitespace();
        }
        JKQTMathTextTextNode* txt1=dynamic_cast<JKQTMathTextTextNode*>(tt1);
        if (txt1) {
            txt1->removeTrailingWhitespace();
        }
    };
    JKQTMathTextNode* t=node;
    for (int iter=0; iter<3; iter++) {
        t=simplifyJKQTMathTextNode(t);
        JKQTMathTextMultiChildNode* nmc=dynamic_cast<JKQTMathTextMultiChildNode*>(t);
        if (nmc && nmc->childCount()>0) {
            bool done=false;
            cleanText(nmc);
            while (!done && nmc->childCount()>0) {
                JKQTMathTextWhitespaceNode* ws0=dynamic_cast<JKQTMathTextWhitespaceNode*>(nmc->getFirstChild());
                JKQTMathTextWhitespaceNode* ws1=dynamic_cast<JKQTMathTextWhitespaceNode*>(nmc->getLastChild());
                if (ws0 && ws0->getWhitespaceType()==JKQTMathTextWhitespaceNode::WSTNormal) {
                    nmc->deleteChild(0);
                } else if (ws1 && ws1->getWhitespaceType()==JKQTMathTextWhitespaceNode::WSTNormal) {
                    nmc->deleteChild(nmc->childCount()-1);
                } else {
                    done=true;
                }
            }
        }
        cleanText(t);
    }
    return t;
}

/*
    Copyright (c) 2008-2022 Jan W. Krieger (<jan@jkrieger.de>)
    with contributions from: Razi Alavizadeh

    

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





#ifndef JKQTMATHTEXT_H
#define JKQTMATHTEXT_H

#include <QFlags>
#include <QObject>
#include <QSettings>
#include <QPainter>
#include <QString>
#include <QSet>
#include <QFile>
#include "jkqtmathtext/jkqtmathtext_imexport.h"
#include "jkqtmathtext/jkqtmathtexttools.h"
#include <QWidget>
#include <QLabel>
#include <QHash>
#include <QPicture>
#include <QImage>
#include <QPixmap>



class JKQTMathTextNode; // forward
class JKQTMathTextVerticalListNode; // forward

/*! \brief this class parses a LaTeX string and can then draw the contained text/equation onto a <a href="http://doc.qt.io/qt-5/qpainter.html">QPainter</a>
    \ingroup jkqtmathtext_render

    
    JKQTMathText is a self-contained LaTeX-renderer for Qt. It is used to renderer
    labels in JKQTPlotter/JKQTBasePlotter, but can be used independently. 
    The class does not depend on any library, except Qt. 
    In particular it actually parses a LaTeX string and draws it in pure C++. It does NOT rely
    on an installed LaTeX for the rendering!

    \see See \ref jkqtmathtext_supportedlatex for a description of the supported LaTeX subset
         and \ref jkqtmathtext_renderingmodel for a description of the rendering model.
    

    \section JKQTMathTextUsage Usage

    \subsection JKQTMathTextUsageDirect Drawing Functions
    The class provides different variants of the drawing function draw() that paints using an
    externlly provided <a href="https://doc.qt.io/qt-6/qpainter.html">QPainter</a>. These variants
    either paint into a QRect, or starting from a single location (x,y).

    The QRect-variants can align the render result inside the rectangle, whereas the
    location-variants draw from a position that is on the left-hand side of the output's baseline:

    \image html jkqtmathtext/jkqtmathtext_node_geo.png



    \subsection JKQTMathTextUsageConvenience Convenience Functions

    Alternatively you can use these methods to directly generate a QPixmap or QPicture:
      - drawIntoPixmap()
      - drawIntoImage()
      - drawIntoPicture()
    .


    \subsection JKQTMathTextSizing Determining the size of an equation

    In addition there are also functions that allow to calculate the size of the equation,
    before drawing it (just like the functions in <a href="http://doc.qt.io/qt-6/qfontmetrics.html">QFontMetrics</a>
    or <a href="http://doc.qt.io/qt-6/qfontmetricsf.html">QFontMetricsF</a>):
      - getSizeDetail()
      - getSize()
      - getAscent(), getDescent()
    .


    \subsection JKQTMathTextUsageQLabel Usage within a QLabel class JKQTMathTextLabel

    Finally, there is also a QLabel-derived class JKQTMathTextLabel which can be used for drawing a LaTeX string onto a Qt form.

    \see JKQTMathTextLabel


    \subsection JKQTMathTextErrorHandling Error Handling

    The class is designed to be as robust as possible and will still return some output, even if the equation contains some errors.
    Nevertheless, several errors are detected while parsing. You can get a list of error messages using getErrorList() after calling parse().
    Also parse() will return \c false if an error occured while parsing.

    \subsection JKQTMathTextUsageExample Example Code
    This small piece of C++ code may serve as an example of the usage and capabilities of the class:
    \code
    // create a JKQTMathText object.
    JKQTMathText mathText;

    // configure its properties to influence the rendering (e.g. fonts to use, font size, ...)
    mathText.useXITS();
    mathText.setFontSize(20);

    // parse some LaTeX code (the Schroedinger's equation)
    mathText.parse("$\\left[-\\frac{\\hbar^2}{2m}\\frac{\\partial^2}{\\partial x^2}+V(x)\\right]\\Psi(x)=\\mathrm{i}\\hbar\\frac{\\partial}{\\partial t}\\Psi(x)$");

    // draw the result into a QPixmap
    QPixmap result=mathText.drawIntoPixmap();
    \endcode

    Alternatively you can also use this class with a <a href="https://doc.qt.io/qt-6/qpainter.html">QPainter</a>:

    \code
    // use the draw() methods to draw the equation using a QPainter (here onto a QPixmap)
    QPainter painter;

    // first we determine the size of the render output:
    const JKQTMathTextNodeSize size=getSizeDetail(painter);

    // now we can us that size information to render:
    mathText.draw(painter, Qt::AlignCenter, QRectF(0,0,pix.width(), pix.height()), false);
    \endcode




    \section JKQTMathTextToHTML Convert to HTML

    The method toHtml() may be used to get a HTML representation of the LaTeX string, if possible (only for simple LaTeX equations!). Whether
    the transformation was possible is returned as a call by value argument!

    
    \section JKQTMathTextExamples Example Projects

    Examples for the usage of this class can be found here: 
      - \ref JKQTMathTextSimpleExample
      - \ref JKQTMathTextRenderCmdLineTool
      - \ref JKQTMathTextTestApp
    .


    \section JKQTMathTextInternalDetails Implementation Details
    \subsection JKQTMathTextSuppoertedFonts Font Handling

    Several fonts are defined as properties to the class:
      - A "roman" (MTEroman / MTEmathRoman) font used as the standard font ( setFontRoman() and for use in math mode setFontMathRoman() )
      - A "sans-serif" (MTEsans / MTEmathSans) font which may be activated with \c \\sf ... ( setFontSans() and for use in math mode setFontMathSans()  )
      - A "typewriter" (MTEtypewriter) font which may be activated with \c \\tt ... ( setFontTypewriter() )
      - A "script" (MTEscript) font which may be activated with \c \\script ... ( setFontScript() )
      - A "math-roman" (MTEmathRoman) font used as the standard font in math mode ( setFontMathRoman() )
      - A "math-sans-serif" (MTEmathSans) used as sans serif font in math mode ( setFontMathSans() )
      - A "blackboard" (MTEblackboard) font used to display double stroked characters ( setFontBlackboard() )
      - A "caligraphic" (MTEcaligraphic) font used to display caligraphic characters ( setFontCaligraphic() )
      - A "fraktur" (MTEfraktur) font used to display fraktur characters ( setFontFraktur() )
      - A fallback font MTEFallbackSymbols for (math) symbols, greek letters ... (if the symbols are not present in the currently used font). ( setFallbackFontSymbols() )
    .

    These fonts are generic font classes, which font is actually used can be configured in JKQTMathText class with the \c set...() functions mentioned above. You can also use these functions to set the fonts used for math rendering in math-mode:
      - useSTIX() use the STIX fonts from <a href="https://www.stixfonts.org/">https://www.stixfonts.org/</a> in math-mode<br>\image html jkqtmathtext/jkqtmathtext_stix.png
      - useXITS() use the XITS fonts from <a href="https://github.com/alif-type/xits">https://github.com/alif-type/xits</a> in math-mode. These are included by default in this library and also activated by default.<br>\image html jkqtmathtext/jkqtmathtext_xits.png
      - useASANA() use the ASANA fonts from <a href="https://ctan.org/tex-archive/fonts/Asana-Math/">https://ctan.org/tex-archive/fonts/Asana-Math/</a> in math-mode<br>\image html jkqtmathtext/jkqtmathtext_asana.png
      - useAnyUnicode() use generic Unicode fonts, e.g. "Arial" and "Times New Roman" in math-mode. You should use fonts that contain as many of the mathematical symbols as possible to ensure good rendering results.<br>using "Times New Roman": \image html jkqtmathtext/jkqtmathtext_timesnewroman.png
        <br>using "Arial": \image html jkqtmathtext/jkqtmathtext_arial.png
        <br>using "Courier New": \image html jkqtmathtext/jkqtmathtext_couriernew.png
        <br>using "Comic Sans MS": \image html jkqtmathtext/jkqtmathtext_comicsans.png
        <br>using "Old English Text": \image html jkqtmathtext/jkqtmathtext_OldEnglish.png
        <br>using "Computer Modern": \image html jkqtmathtext/jkqtmathtext_computermodern.png
        <br>using "Fira": \image html jkqtmathtext/jkqtmathtext_fira.png
        <br>using "MS Segoe UI": \image html jkqtmathtext/jkqtmathtext_mssegoeui.png
        <br>using "DejaVu Sans": \image html jkqtmathtext/jkqtmathtext_DejaVuSans.png
        <br>using "DejaVu Serif": \image html jkqtmathtext/jkqtmathtext_DejaVuSerif.png
    .

    Math-mode is activated by enclosing your equation in \c $...$ or \c \\[...\\] . This mode is optimized for mathematical equations. Here is an example of the difference:
      - <b>math-mode (MTEmathRoman and MTEmathSans, whitespaces are mostly not drawn directly, symbol spacing is different)</b> \c $...$: <br>\image html jkqtmathtext/schreq_mathmode.png
      - <b>normal mode (MTEroman and MTEsans is used, whitespaces are evaluated directly)</b>: <br>\image html jkqtmathtext/schreq_normalmode.png
    .

    Font Lookup for symbols works as follows in JKQTMathTextSymbolNode:
      - if a character is found in the current (or to be used) font, it is taken from there
      - if the character is not found, it is looked for in the fallback fonts MTEFallbackSymbols
      - as a last resort, some symbols can be created otherwise, so if neither of the two options above
        contain the required symbol, the symbol might be synthesized otherwise, or a rectangle with the size of "X" is drawn instead
    .

 */
class JKQTMATHTEXT_LIB_EXPORT JKQTMathText : public QObject {
        Q_OBJECT
    public:
        /** \brief minimum linewidth allowed in a JKQTMathText (given in pt) */
        static const double ABS_MIN_LINEWIDTH;


        /** \brief class constructor */
        JKQTMathText(QObject * parent = nullptr);
        /** \brief class destructor */
        ~JKQTMathText();
        /** \brief load the object settings from the given QSettings object with the given name prefix */
        void loadSettings(const QSettings& settings, const QString& group=QString("mathtext/"));
        /** \brief store the object settings to the given QSettings object with the given name prefix */
        void saveSettings(QSettings& settings, const QString& group=QString("mathtext/")) const;
        /** \brief options for parse() */
        enum ParseOption {
            AddSpaceBeforeAndAfter = 0x01, /*!< \brief If set, a little bit of space is added before and after the text. */
            StartWithMathMode = 0x02, /*!< \brief if set, the parser assumes the LaTeX string is in math-mode (as if surrounded by \c $ ) */
            AllowLinebreaks = 0x04, /*!< \brief If set, linebreak (i.e. \c \\ or \c \\newline )  are allowed, otherwise a single line wihtout such linebreak commands is expected */
            DefaultParseOptions=AllowLinebreaks,
        };
        Q_DECLARE_FLAGS(ParseOptions, ParseOption)
        Q_FLAG(ParseOptions)
        /** \brief parse the given LaTeX string.
         *
         *  \param options Options for parsing, \see ParseOptions
         *  \param allowLinebreaks
         *
         *  \returns \c true on success.
         */
        bool parse(const QString &text, ParseOptions options=DefaultParseOptions);

        /** \brief get the size of the drawn representation. returns an invalid size if no text has been parsed. */
        QSizeF getSize(QPainter& painter);
        /** \brief return the descent, i.e. the distance from the baseline to the lowest part of the representation */
        double getDescent(QPainter& painter);
        /** \brief return the ascentt, i.e. the distance from the baseline to the highest part of the representation */
        double getAscent(QPainter& painter);
        /** \brief return the detailes sizes of the text */
        void getSizeDetail(QPainter& painter, double& width, double& ascent, double& descent, double& strikeoutPos);
        /** \brief return the detailes sizes of the text */
        JKQTMathTextNodeSize getSizeDetail(QPainter& painter);
        /** \brief draw a representation to the  object at the specified position \a x , \a y
         *
         *  \param painter the <a href="http://doc.qt.io/qt-5/qpainter.html">QPainter</a> to use for drawing
         *  \param x position of the left border of the text/expression to be drawn (see sketch below)
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         *
         *  Here is an illustration of the geometry of the drawn text/expression:
         *  \image html jkqtmathtext/jkqtmathtext_node_geo.png
         */
        void draw(QPainter& painter, QPointF x, bool drawBoxes=false);
        /** \brief draw a representation to the  object at the specified position \a x , \a y
         *
         *  \param painter the <a href="http://doc.qt.io/qt-5/qpainter.html">QPainter</a> to use for drawing
         *  \param x x-position of the left border of the text/expression to be drawn (see sketch below)
         *  \param y y-position of the baseline of the text/expression to be drawn (see sketch below)
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         *
         *  Here is an illustration of the geometry of the drawn text/expression:
         *  \image html jkqtmathtext/jkqtmathtext_node_geo.png
         */
        double draw(QPainter& painter, double x, double y, bool drawBoxes=false);
        /** \brief draw into a rectangle \a rect with alignment defined in \a flags (see below)
         *
         *  \param painter the <a href="http://doc.qt.io/qt-5/qpainter.html">QPainter</a> to use for drawing
         *  \param rect rectangle to draw the text/expression into (see sketch below)
         *  \param flags alignment within \a rect (see below), use e.g. <tt>Qt::AlignHCenter | Qt::AlignVCenter</tt> to center the expression inside \a rect
         *               The flags (dark-red is the rectangle \a rect) are interpreted in the following way:
         *               \image html jkqtmathtext/jkqtmathtext_draw_flags.png
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         *
         *
         *
         *  \see drawIntoPixmap(), drawIntoPicture(), getSize(), getSizeDetail()
         */
        void draw(QPainter& painter, unsigned int flags, QRectF rect, bool drawBoxes=false);

        /** \brief render the last  parse result into a <a href="https://doc.qt.io/qt-6/qpixmap.html">QPixmap</a>
         *
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         *  \param backgroundColor fill color for the returnes QPixmap
         *  \param sizeincrease margin around the tight size of the rendering result for the returned QPixmap
         *  \param devicePixelRatio the devicePixelRatio of the returned QPixmap
         */
        QPixmap drawIntoPixmap(bool drawBoxes=false, QColor backgroundColor=QColor(Qt::white), int sizeincrease=0, qreal devicePixelRatio=1.0);

        /** \brief render the last  parse result into a <a href="https://doc.qt.io/qt-6/qimage.html">QImage</a>
         *
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         *  \param backgroundColor fill color for the returnes QPixmap
         *  \param sizeincrease margin around the tight size of the rendering result for the returned QPixmap
         *  \param devicePixelRatio the devicePixelRatio of the returned QImage
         *  \param resolution_dpi resolution in dots/inch
         */
        QImage drawIntoImage(bool drawBoxes=false, QColor backgroundColor=QColor(Qt::white), int sizeincrease=0, qreal devicePixelRatio=1.0, unsigned int resolution_dpi=96);

        /** \brief render the last  parse result into a <a href="https://doc.qt.io/qt-6/qpicture.html">QPicture</a>
         *
         *  \param drawBoxes if \c true boxes defining the size of each node are drawn, example output: \image html jkqtmathtext/jkqtmathtext_drawboxes.png
         */
        QPicture drawIntoPicture(bool drawBoxes=false);


        /** \brief convert LaTeX to HTML. returns \c ok=true on success and \c ok=false else. */
        QString toHtml(bool* ok=nullptr, double fontPointSize=10);

        /** \copydoc fontColor */ 
        void setFontColor(const QColor & __value);
        /** \copydoc fontColor */ 
        QColor getFontColor() const;
        /** \copydoc setFontPointSize() */
        void setFontSize(double __value);
        /** \brief set the default font size in points
         *  \see getFontSize(), fontSize, fontSizeUnits */
        void setFontPointSize(double __value);
        /** \brief set the default font soze in pixels
         *  \see getFontSizePixels(), fontSize, fontSizeUnits */
        void setFontSizePixels(double __value);
        /** \brief returns the currently set default font size in points, if it was defined in points using setFontSize(), or -1 if it was set in pixels with setFontSizePixels()
         *  \see setFontSize(), fontSize, fontSizeUnits */
        double getFontPointSize() const;
        /** \copydoc getFontPointSize() */
        double getFontSize() const;
        /** \brief returns the currently set default font size in pixels, if it was defined in points using setFontSizePixels(), or -1 if it was set in points with setFontSize()
         *  \see setFontSizePixels(), fontSize, fontSizeUnits */
        double getFontSizePixels() const;
        /** \brief add a font pair to the table with font replacements
         *
         * e.g. if it is known that a certain font is not good for rendering, you can add an alternative with this function.
         * These are automatically applied, when setting a new font name!
         *
         * \param nonUseFont the font not to use
         * \param useFont replacement font for nonUseFont
         *
         * The entry in the encodings for this font is kept empty (or even deleted), so the default encoding of the font to be replaced is used!
         */
        void addReplacementFont(const QString& nonUseFont, const QString& useFont);
        /** \brief add a font pair to the table with font replacements
         *
         * e.g. if it is known that a certain font is not good for rendering, you can add an alternative with this function.
         * These are automatically applied, when setting a new font name!
         *
         * \param nonUseFont the font not to use
         * \param useFont replacement font for nonUseFont
         * \param useFontEncoding encoding of the replacement font
         */
        void addReplacementFont(const QString& nonUseFont, const QString& useFont, JKQTMathTextFontEncoding useFontEncoding);
        /** \brief retrieves a replacement for the given font name \a nonUseFont, including its encoding. Returns the given default values \a defaultFont and/or \a defaultFontEncoding if one of the two is not found */
        QPair<QString, JKQTMathTextFontEncoding> getReplacementFont(const QString &nonUseFont, const QString &defaultFont, JKQTMathTextFontEncoding defaultFontEncoding) const;

        /** \brief retrieve the font and encoding to be used for \a font, which might optionally be typeset inside a math environment, specified by in_math_environment, possibly for the given font subclass \a subclass */
        QPair<QString, JKQTMathTextFontEncoding> getFontData(JKQTMathTextEnvironmentFont font, bool in_math_environment=false) const;

        /*! \brief calls setFontRoman(), or calls useXITS() if \a __value \c =="XITS".  calls useSTIX() if \a __value \c =="STIX", ...

            \see setFontRoman(), useXITS(), useSTIX() for more information */
        void setFontRomanOrSpecial(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /*! \brief calls setFontRoman(), or calls useXITS() if \a __value \c =="XITS".  calls useSTIX() if \a __value \c =="STIX", ...

            \see setFontRoman(), useXITS(), useSTIX() for more information */
        void setFontRomanOrSpecial(const JKQTMathTextFontSpecifier & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);

        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEroman   */
        void setFontRoman(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEroman   */
        QString getFontRoman() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEsans   */
        void setFontSans(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEsans   */
        QString getFontSans() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEtypewriter   */
        void setFontTypewriter(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEtypewriter   */
        QString getFontTypewriter() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEscript   */
        void setFontScript(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEscript   */
        QString getFontScript() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEfraktur   */
        void setFontFraktur(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEfraktur   */
        QString getFontFraktur() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEcaligraphic   */
        void setFontCaligraphic(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEcaligraphic   */
        QString getFontCaligraphic() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEblackboard \see setFontBlackboardMode()
         *
         *
         */
        void setFontBlackboard(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \copydoc blackboradFontMode */
        JKQTMathTextBlackboradDrawingMode getFontBlackboradMode() const;
        /** \copydoc blackboradFontMode */
        void setFontBlackboradMode(JKQTMathTextBlackboradDrawingMode mode);

        /** \brief retrieves the font to be used for text in the logical font MTEblackboard \see blackboradFontMode  */
        QString getFontBlackboard() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for symbols in the logical font \a font   */
        void setFallbackFontSymbols(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for symbols in the logical font \a font   */
        QString getFallbackFontSymbols() const;

        /** \brief retrieves the encoding used for the symbol font to be used for symbols   */
        JKQTMathTextFontEncoding getFontEncodingFallbackFontSymbols() const;
        /** \brief retrieves the encoding used for the script font  */
        JKQTMathTextFontEncoding getFontEncodingScript() const;
        /** \brief retrieves the encoding used for the Fraktur font  */
        JKQTMathTextFontEncoding getFontEncodingFraktur() const;
        /** \brief retrieves the encoding used for the typewriter font  */
        JKQTMathTextFontEncoding getFontEncodingTypewriter() const;
        /** \brief retrieves the encoding used for the sans-serif font  */
        JKQTMathTextFontEncoding getFontEncodingSans() const;
        /** \brief retrieves the encoding used for the roman font  */
        JKQTMathTextFontEncoding getFontEncodingRoman() const;
        /** \brief retrieves the encoding used for the blackboard font  */
        JKQTMathTextFontEncoding getFontEncodingBlackboard() const;
        /** \brief retrieves the encoding used for the caligraphic font  */
        JKQTMathTextFontEncoding getFontEncodingCaligraphic() const;


        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEmathRoman   */
        void setFontMathRoman(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEroman   */
        QString getFontMathRoman() const;
        /** \brief set the font \a fontName and it's encoding \a encoding to be used for text in the logical font MTEmathSans   */
        void setFontMathSans(const QString & fontName, JKQTMathTextFontEncoding encoding=JKQTMathTextFontEncoding::MTFEStandard);
        /** \brief retrieves the font to be used for text in the logical font MTEsans   */
        QString getFontMathSans() const;
        /** \brief retrieves the encoding used for the math-mode sans-serif font  */
        JKQTMathTextFontEncoding getFontEncodingMathSans() const;
        /** \brief retrieves the encoding used for the math-mode roman font  */
        JKQTMathTextFontEncoding getFontEncodingMathRoman() const;

        /** \brief configures the class to use the STIX fonts in mathmode
         *
         * use STIX (1.x/2.x) fonts from <a href="https://www.stixfonts.org/">https://www.stixfonts.org/</a> in math-mode
         *
         * \image html jkqtmathtext/jkqtmathtext_stix.png
         */
        bool useSTIX(bool mathModeOnly=true);

        /** \brief configures the class to use the XITS fonts in mathmode
         *
         * use XITS fonts from <a href="https://github.com/alif-type/xits">https://github.com/alif-type/xits</a> in math-mode.
         * These are included by default in this library and also activated by default.
         *
         * \image html jkqtmathtext/jkqtmathtext_xits.png
         *
         * \note The XITS fonts can be compiled into JKQTPlotter, when the CMake-option \c is set to ON (default: ON).
         *       Then the XITS fonts are added as Qt-Ressources to the library binary.
         *       If this is not the case, you have to provide the XITS fonts on the target system by other means, if you want
         *       to use them.
         */
        bool useXITS(bool mathModeOnly=true);

        /** \brief configures the class to use the ASANA fonts in mathmode
         *
         * use the ASANA fonts from <a href="https://ctan.org/tex-archive/fonts/Asana-Math/">https://ctan.org/tex-archive/fonts/Asana-Math/</a> in math-mode
         *
         * \image html jkqtmathtext/jkqtmathtext_asana.png
         */
        bool useASANA(bool mathModeOnly=true);

        /** \brief sets \a timesFont (with its encoding \a encodingTimes ) for serif-text and \a sansFont (with its encoding \a encodingSans ) for both mathmode and textmode fonts
         *
         * \note use generic Unicode fonts, e.g. "Arial" and "Times New Roman" in math-mode.
         *       You should use fonts that contain as many of the mathematical symbols as possible
         *       to ensure good rendering results.
         *
         * <code>useAnyUnicode("Times New Roman", "Times New Roman")</code>:<br>\image html jkqtmathtext/jkqtmathtext_timesnewroman.png  <br><br>
         * <code>useAnyUnicode("Arial", "Arial")</code>:<br>\image html jkqtmathtext/jkqtmathtext_arial.png  <br><br>
         * <code>useAnyUnicode("Courier New", "Courier New")</code>:<br>\image html jkqtmathtext/jkqtmathtext_couriernew.png  <br><br>
         * <code>useAnyUnicode("Comic Sans MS", "Comic Sans MS")</code>:<br>\image html jkqtmathtext/jkqtmathtext_comicsans.png  <br><br>
         * <code>useAnyUnicodeForTextOnly("Comic Sans MS", "Comic Sans MS");</code>:<br/>\image html jkqtmathtext/jkqtmathtext_comicsans_textonly.png
         */
        void useAnyUnicode(QString timesFont, const QString& sansFont, JKQTMathTextFontEncoding encodingTimes=JKQTMathTextFontEncoding::MTFEUnicode, JKQTMathTextFontEncoding encodingSans=JKQTMathTextFontEncoding::MTFEUnicode);
        /** \brief sets \a timesFont (with its encoding \a encodingTimes ) for serif-text and \a sansFont (with its encoding \a encodingSans ) for mathmode fonts only
         *
         * \note use generic Unicode fonts, e.g. "Arial" and "Times New Roman" in math-mode.
         *       You should use fonts that contain as many of the mathematical symbols as possible to ensure good rendering results.
         *
         * \see useAnyUnicodeForTextOnly(), useAnyUnicode()
         */
        void useAnyUnicodeForMathOnly(QString timesFont, const QString& sansFont, JKQTMathTextFontEncoding encodingTimes=JKQTMathTextFontEncoding::MTFEUnicode, JKQTMathTextFontEncoding encodingSans=JKQTMathTextFontEncoding::MTFEUnicode);
        /** \brief sets \a timesFont (with its encoding \a encodingTimes ) for serif-text and \a sansFont (with its encoding \a encodingSans ) for both mathmode fonts only
         *
         *  \see useAnyUnicodeForMathOnly(), useAnyUnicode()
         *
         * <code>useAnyUnicode("Comic Sans MS", "Comic Sans MS")</code>:<br>\image html jkqtmathtext/jkqtmathtext_comicsans.png  <br><br>
         * <code>useAnyUnicodeForTextOnly("Comic Sans MS", "Comic Sans MS");</code>:<br/>\image html jkqtmathtext/jkqtmathtext_comicsans_textonly.png
         */
        void useAnyUnicodeForTextOnly(QString timesFont, const QString& sansFont, JKQTMathTextFontEncoding encodingTimes=JKQTMathTextFontEncoding::MTFEUnicode, JKQTMathTextFontEncoding encodingSans=JKQTMathTextFontEncoding::MTFEUnicode);



        /** \copydoc brace_factor */ 
        void setBraceFactor(double __value);
        /** \copydoc brace_factor */ 
        double getBraceFactor() const;
        /** \copydoc subsuper_size_factor */
        void setSubsuperSizeFactor(double __value);
        /** \copydoc subsuper_size_factor */
        double getSubsuperSizeFactor() const;
        /** \copydoc subsuper_mode_selection_by_size_factor */
        void setSubsuperModeSelectionBySizeFactor(double __value);
        /** \copydoc subsuper_mode_selection_by_size_factor */
        double getSubsuperModeSelectionBySizeFactor() const;
        /** \copydoc italic_correction_factor */
        void setItalicCorrectionFactor(double __value);
        /** \copydoc italic_correction_factor */ 
        double getItalicCorrectionFactor() const;
        /** \copydoc operatorsubsuper_size_factor */ 
        void setOperatorsubsuperSizeFactor(double __value);
        /** \copydoc operatorsubsuper_size_factor */ 
        double getOperatorsubsuperSizeFactor() const;
        /** \copydoc operatorsubsuper_distance_factor */
        void setOperatorsubsuperDistanceFactor(double __value);
        /** \copydoc operatorsubsuper_distance_factor */
        double getOperatorsubsuperDistanceFactor() const;
        /** \copydoc operatorsubsuper_extraspace_factor */
        void setOperatorsubsuperExtraSpaceFactor(double __value);
        /** \copydoc operatorsubsuper_extraspace_factor */
        double getOperatorsubsuperExtraSpaceFactor() const;
        /** \copydoc mathoperator_width_factor */
        void setMathoperatorWidthFactor(double __value);
        /** \copydoc mathoperator_width_factor */
        double getMathoperatorWidthFactor() const;
        /** \copydoc intsubsuper_xcorrection_factor */
        void setIntSubSuperXCorrectionFactor(double __value);
        /** \copydoc intsubsuper_xcorrection_factor */
        double getIntSubSuperXCorrectionFactor() const;
        /** \copydoc intsubbesides_xcorrection_xfactor */
        void setIntSubBesidesXCorrectionXFactor(double __value);
        /** \copydoc intsubbesides_xcorrection_xfactor */
        double getIntSubBesidesXCorrectionXFactor() const;
        /** \copydoc bigmathoperator_font_factor */
        void setBigMathoperatorFontFactor(double __value);
        /** \copydoc bigmathoperator_font_factor */
        double getBigMathoperatorFontFactor() const;
        /** \copydoc super_shift_factor */
        void setSuperShiftFactor(double __value);
        /** \copydoc super_shift_factor */
        double getSuperShiftFactor() const;
        /** \copydoc sub_shift_factor */
        void setSubShiftFactor(double __value);
        /** \copydoc sub_shift_factor */
        double getSubShiftFactor() const;
        /** \copydoc special_super_shift_factor */
        void setSpecialSuperShiftFactor(double __value);
        /** \copydoc special_super_shift_factor */
        double getSpecialSuperShiftFactor() const;
        /** \copydoc special_sub_shift_factor */
        void setSpecialSubShiftFactor(double __value);
        /** \copydoc special_sub_shift_factor */
        double getSpecialSubShiftFactor() const;
        /** \copydoc brace_shrink_factor */
        void setBraceShrinkFactor(double __value);
        /** \copydoc brace_shrink_factor */ 
        double getBraceShrinkFactor() const;
        /** \copydoc underbrace_factor */ 
        void setUnderbraceFactor(double __value);
        /** \copydoc underbrace_factor */ 
        double getUnderbraceFactor() const;
        /** \copydoc underbrace_separation_xfactor */
        void setUnderbraceSeparationXFactor(double __value);
        /** \copydoc underbrace_separation_xfactor */
        double getUnderbraceSeparationXFactor() const;
        /** \copydoc underbrace_bracesize_xfactor */
        void setUnderbraceBraceSizeXFactor(double __value);
        /** \copydoc underbrace_bracesize_xfactor */
        double getUnderbraceBraceSizeXFactor() const;
        /** \copydoc underset_factor */
        void setUndersetFactor(double __value);
        /** \copydoc underset_factor */
        double getUndersetFactor() const;
        /** \copydoc frac_factor */
        void setFracFactor(double __value);
        /** \copydoc frac_factor */
        double getFracFactor() const;
        /** \copydoc frac_nested_factor */
        void setFracNestedFactor(double __value);
        /** \copydoc frac_nested_factor */
        double getFracNestedFactor() const;


        /** \copydoc frac_shift_factor */
        void setFracShiftFactor(double __value);
        /** \copydoc frac_shift_factor */ 
        double getFracShiftFactor() const;
        /** \copydoc brace_y_shift_factor */ 
        void setBraceYShiftFactor(double __value);
        /** \copydoc brace_y_shift_factor */ 
        double getBraceYShiftFactor() const;

        /** \copydoc decoration_height_factor */
        void setDecorationHeightFactor(double __value);
        /** \copydoc decoration_height_factor */
        double getDecorationHeightFactor() const;
        /** \copydoc decoration_separation_factor */
        void setDecorationSeparationXFactor(double __value);
        /** \copydoc decoration_separation_factor */
        double getDecorationSeparationFactor() const;
        /** \copydoc decoration_width_reduction_Xfactor */
        void setDecorationWidthReductionFactor(double __value);
        /** \copydoc decoration_width_reduction_Xfactor */
        double getDecorationWidthReductionXFactor() const;


        /** \copydoc sqrt_width_Xfactor */
        void setSqrtWidthXFactor(double __value);
        /** \copydoc sqrt_width_Xfactor */
        double getSqrtWidthXFactor() const;
        /** \copydoc sqrt_height_factor */
        void setSqrtHeightFactor(double __value);
        /** \copydoc sqrt_height_factor */
        double getSqrtHeightFactor() const;
        /** \copydoc sqrt_smallfont_factor */
        void setSqrtSmallFontFactor(double __value);
        /** \copydoc sqrt_smallfont_factor */
        double getSqrtSmallFontFactor() const;

        /** \copydoc matrix_linewidth_thin_factor */
        double getMatrixLinewidthThinFactor();
        /** \copydoc matrix_linewidth_thin_factor */
        void setMatrixLinewidthThinFactor(double factor);
        /** \copydoc matrix_linewidth_heavy_factor */
        double getMatrixLinewidthHeavyFactor();
        /** \copydoc matrix_linewidth_heavy_factor */
        void setMatrixLinewidthHeavyFactor(double factor);
        /** \copydoc matrix_line_separation_factor */
        double getMatrixLineSeparationFactor();
        /** \copydoc matrix_line_separation_factor */
        void setMatrixLineSeparationFactor(double factor);
        /** \copydoc matrix_xSeparation_factor */
        double getMatrixXSeparationFactor();
        /** \copydoc matrix_xSeparation_factor */
        void setMatrixXSeparationFactor(double factor);
        /** \copydoc matrix_ySeparation_factor */
        double getMatrixYSeparationFactor();
        /** \copydoc matrix_ySeparation_factor */
        void setMatrixYSeparationFactor(double factor);
        /** \copydoc matrix_xPadding_factor */
        double getMatrixXPaddingFactor();
        /** \copydoc matrix_xPadding_factor */
        void setMatrixXPaddingFactor(double factor);
        /** \copydoc matrix_yPadding_factor */
        double getMatrixYPaddingFactor();
        /** \copydoc matrix_yPadding_factor */
        void setMatrixYPaddingFactor(double factor);


        /** \copydoc error_list */
        QStringList getErrorList() const;
        /** \brief returns \c true when errors were registered in the system \see error_list */
        bool hadErrors() const;
        /** \copydoc error_list */
        void addToErrorList(const  QString& error);



    protected:
        /** \brief table with font replacements to use (e.g. if it is known that a certain font is not good for rendering, you can add
         *         an alternative using addReplacementFont(). These are automatically applied, when setting a new font name! */
        QMap<QString, QString> fontReplacements;
        /** \brief acompanies fontReplacements and collects the encodings of the replacement fonts, if no entry is present, the default encoding is used, as given to the setter! */
        QMap<QString, JKQTMathTextFontEncoding> fontEncodingReplacements;

        /** \brief font color */
        QColor fontColor;
        /** \brief base font size in the units defined in fontSizeUnits \see fontSizeUnits */
        double fontSize;
        /** \brief unit of fontSize */
        JKQTMathTextEnvironment::FontSizeUnit fontSizeUnits;


        /** \brief stores information about the different fonts used by LaTeX markup */
        QHash<JKQTMathTextEnvironmentFont, JKQTMathTextFontDefinition> fontDefinitions;

        /** \brief specifies how to draw blackboard font characters (i.e. \c \\mathbb{N} )
         *
         *  Blackboard fonts are not widely available on target systems (viable fonts are e.g.
         *  <code>"Double Stroke", "CloisterOpenFace BT", "GoudyHandtooled BT", "Castellar", "MathJax_AMS", "Castellar Standard", "MathJax_AMS Standard", "Colonna MT"</code>).
         *  But the most important blackboard characters are usually available in the higher unicode
         *  codepoints of Fonts specialized for math (e.g. XIST, STIX, ASANA).
         *
         *  Therefore JKQTMathText supports using these characters, or simulating a blackboard font in
         *  addition to using one of the fonts above. You can set that by setting
         *  JKQTMathText::setFontBlackboradMode() with one of the options from JKQTMathTextBlackboradDrawingMode:
         *    - MTBBDMfontDirectly: use a blackboard font specified by JKQTMathText::setFontBlackboard() \image html jkqtmathtext/jkqtmathtext_bb_font_directly.png using \c JKQTMathText::setFontBlackboard("Castellar")
         *    - MTBBDMsimulate: \image html jkqtmathtext/jkqtmathtext_bb_simulate.png using \c JKQTMathText::setFontBlackboard("Arial")
         *    - MTBBDMunicodeCharactersOrFontDirectly: \image html jkqtmathtext/jkqtmathtext_bb_unicode_or_font_directly.png using \c JKQTMathText::setFontBlackboard("Castellar")
         *    - MTBBDMunicodeCharactersOrSimulate: \image html jkqtmathtext/jkqtmathtext_bb_unicode_or_simulate.png using \c JKQTMathText::setFontBlackboard("Arial")
         *  .
         *
         *  \see setFontBlackboard() setBlackboardFontMode()
         */
        JKQTMathTextBlackboradDrawingMode blackboradFontMode;


        /** \brief resizing factor for braces in math mode */
        double brace_factor;
        /** \brief shrinking the width of braces in math mode 0: reduce to 0 pixel width, 1: leave unchanged*/
        double brace_shrink_factor;
        /** \brief resizing factor for font size in sub-/superscript */
        double subsuper_size_factor;
        /** \brief this factor is used to determine how to typeset sub-/superscript.
         *
         *  If the ascent for superscript of descent for subscript of the previous character is  \c >=subsuper_mode_selection_by_size_factor*ascent(currentFont)
         *  or \c <=subsuper_mode_selection_by_size_factor*descent(currentFont) respectively, the sub/superscript is typeset, aligned with the ascent or descent
         *  of the previous character. Otherwise it is aligned with the default method:
         *
         *  <b>Default mode:</b>
         *  \image html jkqtmathtext_superscriptnode_getSizeInternal.png
         *  \image html jkqtmathtext_subscriptnode_getSizeInternal.png
         *
         *  <b>Special mode:</b>
         *
         *
         *  This method fixes problems with characters that are significantyl larger that normal text character of the fonst, such as generated
         *  by \c \\sum,\\int,... .
         */
        double subsuper_mode_selection_by_size_factor;
        /** \brief fraction of a whitespace by which to shift a sub-/superscript left/right when the previous text is italic */
        double italic_correction_factor;
        /** \brief like subsuper_size_factor, but for operators (\c \\sum , \c \\int , ...) where the text is placed above/below the symbol */
        double operatorsubsuper_size_factor;
        /** \brief for operators (\c \\sum , \c \\int , ...) where the text is placed above/below the symbol, this is the distance between the operator symbol and the sub-/super-text if multiplied by xHeight
         *
         *  \image html jkqtmathtext_subsuper_with_limits.png
         */
        double operatorsubsuper_distance_factor;
        /** \brief for operators (\c \\sum , \c \\int , ...) where the text is placed above/below the symbol, this is the additional width added to the width of maximum width of the operator, above and below
         *
         *  \image html jkqtmathtext_subsuper_with_limits.png
         */
        double operatorsubsuper_extraspace_factor;
        /** \brief for integrals (\c \\int , \c \\oint , ...) the sub-/superscripts above/below the symbol have to be shifted a bit to the left/right to accomodate the shape of the operator symbol (i.e. some free space at the top-left and bottom-right)
         *
         *  This factor is multiplied by the symbol width: xshift=intsubsuper_xcorrection_factor*symbolWidth
         *  Then the subscript below is placed at centerx(symbol)-xshift and the superscript at centerx(symbol)+shiftx.
         *  This is also used to correct a subset next to the symbol by shifting it to rightx(symbol)-xshift.
         *
         *  This correction is applied to \\int, \\iint, \\iiint, \\oint, ...
         */
        double intsubsuper_xcorrection_factor;
        /** \brief for integrals (\c \\int , \c \\oint , ...) the subscripts besides the symbol have to be shifted to the left a bit to the left to accomodate the shape of the operator symbol (i.e. some free space at the bottom-right)
         *
         *  This factor is multiplied by the width of an x: xshift=intsubbesides_xcorrection_xfactor*xWidth
         *  Then the subscript besides the symbol is shifted by xshift to the left
         *
         *  This correction is applied to \\int, \\iint, \\iiint, \\oint, ...
         */
        double intsubbesides_xcorrection_xfactor;
        /** \brief factor, used to extend the size of an operator in math mode
         *
         *  The next image demonstrates the effect of this property, which adds extra space
         *  around certain math operators in math mode:
         *
         *  \image html jkqtmathtext/jkqtmathtext_mathoperator_width_factor.png
         */
        double mathoperator_width_factor;
        /** \brief factor, used to increase the font size for big math operators, such as \c \\sum , \c \\prod , ...
         *
         */
        double bigmathoperator_font_factor;
        /** \brief relative shift of text in superscript to normal text, factor is multiplied by \c font.xHeight()
         *
         *  \image html jkqtmathtext_superscriptnode_getSizeInternal.png
         */
        double super_shift_factor;
        /** \brief relative shift of text in subscript to normal text, factor is multiplied by \c font.xHeight()
         *
         *  \image html jkqtmathtext_subscriptnode_getSizeInternal.png
         */
        double sub_shift_factor;
        /** \brief relative shift of text in superscript to normal text in special superscript mode (after large previous nodes):
         *         0= superscript descent coincides with the previous node's baselineHeight, 1: top of previous node and top of the superscript nodes coincide
         *
         *  \image html jkqtmathtext_specialsuperscriptnode_getSizeInternal.png
         */
        double special_super_shift_factor;
        /** \brief relative shift of text in subscript to normal text in special superscript mode (after large previous nodes):
         *         0=child's baseline at the descent of the previous node, 1: subscript-node starts at the descent of the previous node
         *
         *  \image html jkqtmathtext_specialsubscriptnode_getSizeInternal.png
         */
        double special_sub_shift_factor;


        /** \brief scaling factor for font size of nominator and denominator of a fraction
         *
         *  \image html jkqtmathtext/jkqtmathtext_frac_factor.png
         */
        double frac_factor;
        /** \brief scaling factor for font size of nominator and denominator of a nested fraction
         *
         *  \image html jkqtmathtext/jkqtmathtext_frac_factor.png
         */
        double frac_nested_factor;
        /** \brief shift of denominator/nummerator away from central line of a frac
         *
         *  \image html jkqtmathtext_fracnode_geo.png
         */
        double frac_shift_factor;
        /** \brief scaling factor for font of underbrace/overbrace text */
        double underbrace_factor;
        /** \brief scaling factor for font of underset/overset text */
        double underset_factor;
        /** \brief additional space between the main text to the curly brace and the brace to underbrace/overbrace, multiplied with height("x") */
        double underbrace_separation_xfactor;
        /** \brief height of the brace in underbrace/overbrace, multiplied with ascent */
        double underbrace_bracesize_xfactor;
        /** \brief fraction of the brace ascent that the brace is shifted downwards, when scaled */
        double brace_y_shift_factor;
        /** \brief size of the decorations (dot, tilde, ...), as fraction of the ascent
         *
         *  \image html jkqtmathtext/decoration_sizing.png
         */
        double decoration_height_factor;
        /** \brief separation between the text and the decorations (dot, tilde, ...), as fraction of the ascent
         *
         *  \image html jkqtmathtext/decoration_sizing.png
         */
        double decoration_separation_factor;
        /** \brief a decoration has a size, which is slightly smaller than the text- width. the width is reduced by \c decoration_width_reduction_Xfactor*width("X") and the position is centered around the child-box. Also an italic correction is applied:
         *
         *  \image html jkqtmathtext/decoration_sizing.png
         */
        double decoration_width_reduction_Xfactor;
        /** \brief scaling factor for the small font used to indicate the degree of the sqrt */
        double sqrt_smallfont_factor;
        /** \brief width of the sqrt-symbol, as factor to width("X") */
        double sqrt_width_Xfactor;
        /** \brief height-increase of the sqrt-symbol, as factor of the child's height */
        double sqrt_height_factor;

        /** \brief width of thin table lines, this factor is multiplied with the current \c font.linewidth()
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_linewidth_thin_factor;
        /** \brief width of heavy table lines, this factor is multiplied with the current \c font.linewidth()
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_linewidth_heavy_factor;
        /** \brief separation of double-lines in tables, this factor is multiplied with the \c font.linewidth()
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_line_separation_factor;
        /** \brief x-separation of  two columns, this factor is multiplied with the \c font.width("x")
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_xSeparation_factor;
        /** \brief y-separation of  two columns, this factor is multiplied with the \c font.width("x")
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_ySeparation_factor;
        /** \brief x-padding of  two columns, this factor is multiplied with the \c font.width("x")
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_xPadding_factor;
        /** \brief y-padding of  two columns, this factor is multiplied with the \c font.width("x")
         *
         *  \image html jkqtmathtext/jkqtmathtext_matrix_geometry.png
         */
        double matrix_yPadding_factor;

        /** \brief a list that will be filled with error messages while parsing, if any error occur
         *
         *  This list of errors is (mostly) filled during a call to parse(). During rendering (e.g. with draw() )
         *  only very few errors will be detected, as most errors are caused by wrong markup.
         *
         *  A call to parse() also clears this list.
         *
         *  \see getErrorList(), hadErrors() and addToErrorList()
         *
          */
        QStringList error_list;

        /** \brief the result of parsing the last string supplied to the object via parse() */
        JKQTMathTextNode* parsedNode;

        /** \brief returns the syntax tree of JKQTMathTextNode's that was created by the last parse call */
        JKQTMathTextNode* getNodeTree() const;

        /** \brief the token types that may arrise in the string */
        enum tokenType {
            MTTnone, /*!< \brief no token */
            MTTtext, /*!< \brief a piece of general text */
            MTTinstruction, /*!< \brief an instruction, started by \c "\", e.g. \c "\\textbf", ... */
            MTTinstructionNewline,   /*!< \brief a newline instruction \c "\\" */
            MTTinstructionVerbatim,  /*!< \brief a verbatim instruction, e.g. \c \\verb!verbatimtext! was found: currentTokenName will contain the text enclode by the verbatim delimiters */
            MTTinstructionVerbatimVisibleSpace,  /*!< \brief a verbatim instruction that generates visible whitespaces, e.g. \c \\begin{verbatim}...\\end{verbatim} was found: currentTokenName will contain the text enclode by the verbatim delimiters */
            MTTinstructionBegin, /*!< \brief a \c '\\begin{...}' instruction, currentTokenName is the name of the environment */
            MTTinstructionEnd, /*!< \brief a \c '\\end{...}' instruction, currentTokenName is the name of the environment */
            MTTunderscore,  /*!< \brief the character \c "_" */
            MTThat,  /*!< \brief the character \c "^" */
            MTTdollar,  /*!< \brief the character \c "$" */
            MTTopenbrace, /*!< \brief the character \c "{" */
            MTTclosebrace, /*!< \brief the character \c "}" */
            MTTopenbracket, /*!< \brief the character \c "[" */
            MTTclosebracket, /*!< \brief the character \c "]" */
            MTTwhitespace, /*!< \brief some whitespace */
            MTTampersand,  /*!< \brief the character \c "&" */
            MTThyphen,  /*!< \brief the single hyphen character \c "-" in text-mode \note MTTendash and MTTemdash take precedence over MTThypen  */
            MTTendash,  /*!< \brief the en-dash character sequence \c "--" in text-mode */
            MTTemdash,  /*!< \brief the em-dash character sequence \c "---" in text-mode */

        };
        /** \brief convert a tokenType into a string, e.g. for debugging output */
        static QString tokenType2String(tokenType type);

        /** \brief tokenizer for the LaTeX parser */
        tokenType getToken();
        /** \brief returns some characters to the Tokenizer */
        void giveBackToTokenizer(size_t count);
        /** \brief parse a LaTeX string
         *
         *  \param get if \c true this calls getToken()
         *  \param quitOnClosingBrace if unequal MTBTAny, this returns if the given closing brace is found
         *  \param quitOnEnvironmentEnd wuit if \c \\end{quitOnEnvironmentEnd} is found
         *  \param quitOnClosingBracket if \c true, quits on encountering a MTTclosebracket token
         */
        JKQTMathTextNode* parseLatexString(bool get, JKQTMathTextBraceType quitOnClosingBrace=JKQTMathTextBraceType::MTBTAny, const QString& quitOnEnvironmentEnd=QString(""), bool quitOnClosingBracket=false);
        /** \brief parse a LaTeX string with linebreaks
         *
         *  \param get if \c true this calls getToken()
         *  \param quitOnEnvironmentEnd wuit if \c \\end{quitOnEnvironmentEnd} is found
         *  \param _alignment horizontal alignment of the JKQTMathTextVerticalListNode \see JKQTMathTextVerticalListNode::alignment and JKQTMathTextHorizontalAlignment
         *  \param _linespacingFactor line spacing factor of the lines \see JKQTMathTextVerticalListNode::linespacingFactor
         *  \param spacingMode_ spacing mode/algorithm for the lines \see JKQTMathTextLineSpacingMode and JKQTMathTextLineSpacingMode
         *  \param _verticalOrientation vertical orientation of the block of all lines, see JKQTMathTextVerticalListNode::verticalOrientation and JKQTMathTextVerticalOrientation
         *
         *  \returns JKQTMathTextVerticalListNode with the lines as children
         */
        JKQTMathTextVerticalListNode *parseMultilineLatexString(bool get, const QString& quitOnEnvironmentEnd=QString(""), JKQTMathTextHorizontalAlignment _alignment=MTHALeft, double _linespacingFactor=1.0, JKQTMathTextLineSpacingMode spacingMode_=MTSMDefaultSpacing, JKQTMathTextVerticalOrientation _verticalOrientation=MTVOFirstLine);
        /** \brief parses a list of string-arguments, i.e. \c {p1}{p2}{...}
         *
         *  \param get call getToken() at the start, otherwise it is expected that currentToken==MTTopenbrace
         *  \param Nparams the number of parameters to expect
         *  \param[out] foundError will be set to \c true if an error occured (unexpected token) or \c false otherwise
         *  \return the list of parameter strings with Nparam entries or an empty or partial list on error
         */
        QStringList parseStringParams(bool get, size_t Nparams, bool *foundError=nullptr);
        /** \brief parses a string, i.e. a sequence of text and whitespaces. returns after any other token was found */
        QString parseSingleString(bool get);
        /** \brief read all text without tokenizing, until the sequence \a endsequence is found.
         *
         *  \param get if \c true the functions begins by reading a new character, otherwise the current character is used as first character
         *  \param endsequence the sequence, ending the read
         *  \param removeFirstlineWhitespace if \c true the returned string does not contain the first whitespace-line and a possible trailing whitespace line
         *  \return the read string, excluding the  \a endsequence
         */
        QString readUntil(bool get, const QString& endsequence, bool removeFirstlineWhitespace=false);
        /** \brief parses a single instruction (including it's parameters)
         *
         *  \param[out] _foundError will be set to \c true if an error occured (unexpected token) or \c false otherwise
         *  \param[out] getNew returns \c true if the parser has to call getToken() to go on
         *  \return the instruction node or \c nullptr on error (then also \a _foundError is set \c true )
         *  \note This method expects the current token currentToken to be MTTinstruction
         */
        JKQTMathTextNode* parseInstruction(bool *_foundError=nullptr, bool* getNew=nullptr);
        /** \brief parse a LaTeX math environment */
        JKQTMathTextNode* parseMath(bool get);

        /** \brief used by the tokenizer. type of the current token */
        tokenType currentToken;
        /** \brief the JKQTMathTextBraceType associated with the last \c \\right command the parser encountered */
        JKQTMathTextBraceType lastRightBraceType;
        /**  \brief returns the number of \c \\hline , \c \\hdashline , ... commands in the last parseLatexString() call */
        QMap<QString,size_t> lastLineCount;
        /** \brief used by the tokenizer. Name of the current token, id applicable */
        QString currentTokenName;
        /** \brief used by the tokenizer. Points to the currently read character in parseString */
        int currentTokenID;
        /** \brief used by the tokenizer. The string to be parsed */
        QString parseString;
        /** \brief used by the parser. indicates whether we are in a math environment */
        bool parsingMathEnvironment;


    public:
        /** \copydoc parsedNode */ 
        JKQTMathTextNode *getParsedNode() const;

};


#endif // JKQTMATHTEXT_H










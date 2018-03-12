
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * SKINS LIBRARY
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


pergola.skins = {
	rubber : function () {
    var defs = pergola.defs,
        shade = pergola.color.shade,
        gradient,
        pattern,
				luminosity;

    pergola.color.lab(pergola.color.convert(pergola.theme));
    luminosity = pergola.themeLuminosity;

    pergola.presentationAttributes = {
      all : {
        'font-family' : "'Segoe UI', 'Trebuchet MS', 'Lucida Grande', 'Deja Vu'",
        'font-size' : 13,
        stroke : "#7F7F7F"
      },
      background : {
        fill : "white",
        stroke : "none",
        opacity : .5,
        'xlink:href' : ""
      },
      button : {
        fill : "url(#buttonGrad)",
        stroke : "#878787",
        maskFill : "url(#buttonMaskGrad)",
        large : {
          width : 20,
          height : 20
        },
        small : {
          width : 14,
          height : 14
        }
      },
      checkBox : {
        width : 13,
        height : 13,
        fill : "white"
      },
      colorBoxSelector : {
        fill : "#FFFFFF",
        stroke : "#B0B0B0",
        width : 216,
        height : 21
      },
      comboBox : {
        width : 220,
        height : 160,
        listItemHeight : 21,
        listMargin : 8
      },
      commandButton : {
        width : 15,
        height : 13,
        rx : 3
      },
      coordinates : {
        "font-size" : 12
      },
      dialogButton : {
        width : 64,
        height : 25,
        rx : 3
      },
      frame : {
        width : 200,
        height : 200,
        fill : shade([90, 90, 90]),
        stroke : shade([90, 90, 90], -24),
        'fill-opacity' : function () {
          var c = pergola.color.hexToRGB(pergola.presentationAttributes.background.fill);
          var lum = pergola.color.luminosity(c[0], c[1], c[2]);
          return (.2 + .15 * (lum < luminosity));
        },
        'stroke-opacity' : function () {
          var c = pergola.color.hexToRGB(pergola.presentationAttributes.background.fill);
          var lum = pergola.color.luminosity(c[0], c[1], c[2]);
          return (.3 + .3 * (lum < luminosity));
        },
        rx : 3
      },
      input : {
        width : 36,
        height : 20,
        fill : "#FFFFFF",
        stroke : "#A0A0A0",
        hasButtons : true
      },
      inputCursor : {
        blinkRate : 750
      },
      legend : {
        'font-size' : '12px'
      },
      listItem : {
        fill : "url(#listItemGrad)",
        maskFill : "#FBFBFB"
      },
      menu : {
        'font-size' : '12px',
        fill : "url(#menuGrad)",
        stroke : shade([100, 100, 100], -4),
        rx : 3,
        opacity : 0.4
      },
      menuBar : {
      	"font-size" : "12px",
        height : 19,
        fill : "url(#menubarGrad)"
      },
      menuItem : {
        height : 19,
        fill : "url(#listItemGrad)"
      },
      menuList : {
        margin : 15,
        filter : pergola.filter.blur({stdDeviation : 1.4, dx : 2.8, dy : 2.8, "flood-opacity" : .42})
      },
      notification : {
        x : 440,
        y : 340,
        width : 360,
        height : 220,
        fill : "white"
      },
      panel : {
        x : 150,
        y : 120,
        width : 440,
        height : 360,
        margin : 4,
        fill : "#ebecec",
        footerFill : "#ebecec",
        footerHeight : 60,
        filter : pergola.filter.blur()
      },
      popupList : {
        width : 200,
        listItemHeight : 20,
        listMargin : 8
      },
      preview : {
        rx : 2,
        width : 150,
        height : 120,
        opacity : .5
      },
      progress : {
        fill : "#F0F0F0",
        stroke : "#E0E0E0",
        statusFill : "url(#progressGrad)",
        width : 300,
        height : 12
      },
      quickTip : {
        'font-size' : '8pt',
        rx : 6,
        fill : "url(#quickTipGrad)",
        stroke : "#989898",
        textFill : "#242424",
        delay : 700
      },
      radioButton : {
        r : 7,
        dotR : 3,
        fill : "url(#radioButtonGrad)",
        stroke : "#808080",
        dotFill : "#303030"
      },
      rubberBox : {
        stroke : "black",
        'stroke-width' : 1,
        'stroke-dasharray' : "1,2",
        maskStroke : "white"
      },
      scrollBar : {
        fill : shade([100, 100, 100], 40),
        stroke : "#D0D0D0",
        horTrackFill : "url(#hTrackGrad)",
        vertTrackFill : "url(#vTrackGrad)",
        rx : 0,
        size : 15,
        padding : 1.5,
        delay : 250,
        stepFrequence : 50,
        step : 40,
        pageFrequence : 100,
        mapPageFrequence : 500
      },
      scrollSlider : {
        fill : {
          horizontal : "url(#sliderPattern)",
          vertical : "url(#sliderPattern)"
        },
        rx : 0,
        stroke : {
          horizontal : shade([100, 100, 100]),
          vertical : shade([100, 100, 100])
        }
      },
      selector : {
        'font-size' : "9pt",
        fill : "#F5F5F6",
        stroke : "#B0B0B0",
        maskFill : "#B0B0B0",
        'stroke-width' : 2,
        rx : 5,
        width : 216,
        height : 21
      },
      separator : {
        width : 13,
        height : 5
      },
      slider : {
        'font-size' : "7pt",
        rx : 2,
        fill : "url(#sliderButtonGrad)",
        stroke : "#FFFFFF",
        trackFill : "#C0C0C0",
        trackStroke : "none",
        trackOpacity : .05,
        slotStroke : "#606060",
        slotSize : 1,
        shape : "round",
        size : 16,
        type : "linear",
        initial : "middle",
        valueTip : false
      },
      statusBar : {
        fill : "url(#statusBarGrad)",
        height : 20,
        "font-size" : "10px",
        advanceX : 10
      },
      tab : {
        'font-size' : '8pt',
        fill : "url(#tabGrad)",
        'fill-opacity' : .5,
        width : 120,
        height : 18.5
      },
      taskBar : {
        fill : "url(#taskbarGrad)",
        maskFill : shade([100, 100, 100], 20),
        width : "100%",
        height : 32,
        position : "top",
        hasMenu : false
      },
      text : {
        fill : (luminosity < 128)? "white" : "#000000",
        fillInverse : (luminosity < 128)? "#000000" : "#FFFFFF",
        shadow : (luminosity < 128)? "#000000" : shade([82, 84, 85.5], luminosity / 4)
      },
      toolBar : {
        fill : "url(#toolbarGrad)",
        height : 29,
        rx : 0,
        spacing : 5
      },
      toolButton : {
        fill : "url(#buttonGrad)",
        stroke : "url(#toolButtonStrokeGrad)",
        maskFill : "url(#toolButtonMaskGrad)",
        rx : 6
      },
      topBar : {
        fill : {
          panel : "url(#topBarGrad)",
          window : "url(#winTopBarGrad)",
          messageBox : "url(#alertTopBarGrad)"
        },
        maskFill : shade([100, 100, 100], 25),
        height : {
          panel : 32,
          window : 44
        }
      },
      window : {
        fill : "#FFFFFF",
        scrollSize : 17
      },
      windowTab : {
        'font-size' : 11,
        width : 100,
        fill : "url(#windowTabGrad)",
        maskFill : "url(#windowTabMaskGrad)",
        maskStroke : shade([100, 100, 100], 75)
      }
    };

    gradient = $C({element : "linearGradient", id : "taskbarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#FFFFFF", "stop-opacity" : .9, appendTo : gradient});
    $C({element : "stop", offset : "4%", "stop-color" : "#D8D8D8", "stop-opacity" : .65, appendTo : gradient});
    $C({element : "stop", offset : "95%", "stop-color" : "#606060", "stop-opacity" : .4, appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#000000", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "windowTabGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([90, 90, 90], 20), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([90, 90, 90], 50), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "windowTabMaskGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([90, 90, 90], 20), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([90, 90, 90], 0), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "topBarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -18), appendTo : gradient});
    $C({element : "stop", offset : "10%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "94%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 60), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "alertTopBarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -36), appendTo : gradient});
    $C({element : "stop", offset : "10%", "stop-color" : shade([100, 100, 100], -16), appendTo : gradient});
    $C({element : "stop", offset : "95%", "stop-color" : shade([100, 100, 100], -8), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 80), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "winTopBarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -40), appendTo : gradient});
    $C({element : "stop", offset : "1%", "stop-color" : shade([100, 100, 100], -18), appendTo : gradient});
    $C({element : "stop", offset : "12%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "92%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 32), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "menuGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 50), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 45), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "menubarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([103.333, 98.514, 92.934]), appendTo : gradient});
    $C({element : "stop", offset : "44%", "stop-color" : shade([104.761, 99.009, 91.847]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([102.38, 91.584, 80.434]), appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "radioButtonGrad", cx : "65%", cy : "65%", r : "100%",  fx : "65%", fy : "65%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#F0F0F0", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#A8B0B8", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "tabGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#C0C0C0", appendTo : gradient});
    $C({element : "stop", offset : "15%", "stop-color" : "#ECECEC", appendTo : gradient});
    $C({element : "stop", offset : "50%", "stop-color" : "white", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "toolbarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([102.333, 100, 96.934]), appendTo : gradient});
    $C({element : "stop", offset : "44%", "stop-color" : shade([103.761, 100.009, 97.847]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([99.38, 92.584, 88.434]), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "statusBarGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 70), appendTo : gradient});
    $C({element : "stop", offset : "8%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "94%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -30), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "toolButtonMaskGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([104.761, 99.009, 91.847], 30), appendTo : gradient});
    $C({element : "stop", offset : "44%", "stop-color" : shade([104.761, 99.009, 91.847], 36), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([104.761, 99.009, 91.847], 21), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "toolButtonStrokeGrad", x1 : "0%", y1 : "0%", x2 : "33%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 100), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 60), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "hTrackGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -35), appendTo : gradient});
    $C({element : "stop", offset : "12%", "stop-color" : shade([100, 100, 100], 78), appendTo : gradient});
    $C({element : "stop", offset : "88%", "stop-color" : shade([100, 100, 100], 78), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FFFFFF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "listItemGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([120.283, 61.538, 47.058], 9), appendTo : gradient});
    $C({element : "stop", offset : "45%", "stop-color" : shade([120.283, 61.538, 47.058], 11), appendTo : gradient});
    $C({element : "stop", offset : "55%", "stop-color" : shade([120.283, 61.538, 47.058], 10), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([120.283, 61.538, 47.058], -5), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "vTrackGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -35), appendTo : gradient});
    $C({element : "stop", offset : "12%", "stop-color" : shade([100, 100, 100], 78), appendTo : gradient});
    $C({element : "stop", offset : "88%", "stop-color" : shade([100, 100, 100], 78), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FFFFFF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "hScrollSliderStrokeGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 74), appendTo : gradient});
    $C({element : "stop", offset : "10%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "88%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -25), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "vScrollSliderStrokeGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 74), appendTo : gradient});
    $C({element : "stop", offset : "10%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "88%", "stop-color" : shade([100, 100, 100]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -25), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "buttonMaskGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 76), appendTo : gradient});
    $C({element : "stop", offset : "8%", "stop-color" : shade([100, 100, 100], 55), appendTo : gradient});
    $C({element : "stop", offset : "88%", "stop-color" : shade([100, 100, 100], 55), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -12), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "buttonGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], -20), appendTo : gradient});
    $C({element : "stop", offset : "20%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "80%", "stop-color" : shade([100, 100, 100], 10), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 55), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "neutralButtonMaskGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#F8F8F8", appendTo : gradient});
    $C({element : "stop", offset : "20%", "stop-color" : "#DADCDE", appendTo : gradient});
    $C({element : "stop", offset : "80%", "stop-color" : "#DADCDE", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#9EA0A2", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "neutralButtonGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#8A8C8E", appendTo : gradient});
    $C({element : "stop", offset : "20%", "stop-color" : "#B4B6B8", appendTo : gradient});
    $C({element : "stop", offset : "80%", "stop-color" : "#B4B6B8", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FFFFFF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "sliderButtonGrad", x1 : "0%", y1 : "0%", x2 : "40%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 80), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -8), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "sliderPatternGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([100, 100, 100], 20), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], -20), appendTo : gradient});

    pattern = $C({element : "pattern", id : "sliderPattern", x : 2.5, y : 2.5, width : 5, height : 5, patternUnits : "userSpaceOnUse", appendTo : defs});
    $C({element : "rect", x : 0, y : 0, width : 5, height : 5, fill : "url(#sliderPatternGrad)", appendTo : pattern});
    $C({element : "circle", cx : 2.5, cy : 2.5, r : 1.5, fill : shade([100, 100, 100]), appendTo : pattern});

    gradient = $C({element : "linearGradient", id : "horScrollSliderGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([102.333, 100, 96.934], 20), appendTo : gradient});
    $C({element : "stop", offset : "44%", "stop-color" : shade([103.761, 100.009, 97.847]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([99.38, 92.584, 88.434]), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "vertScrollSliderGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : shade([102.333, 100, 96.934], 20), appendTo : gradient});
    $C({element : "stop", offset : "44%", "stop-color" : shade([103.761, 100.009, 97.847]), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([99.38, 92.584, 88.434]), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "resizeGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "40%", "stop-color" : "#FFFFFF", "stop-opacity" : .05, appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#000000", "stop-opacity" : .25, appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "lensGrad", cx : "28%", cy : "20%", r : "54.6%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "12%", "stop-color" : "#FAFAFA", appendTo : gradient});
    $C({element : "stop", offset : "34%", "stop-color" : "#D8E0E2", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#ACB9BD", appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "handGrad", cx : "55%", cy : "65%", r : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "20%", "stop-color" : "#FFFFFF", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#ACB9BD", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "progressGrad", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "50%", "stop-color" : "#d00030", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#ffc030", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "quickTipGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "10%", "stop-color" : shade([100, 100, 100], 100), appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : shade([100, 100, 100], 72), appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "insetStroke", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#c0c0c0", appendTo : gradient});
    $C({element : "stop", offset : "49.9%", "stop-color" : "#c0c0c0", appendTo : gradient});
    $C({element : "stop", offset : "50%", "stop-color" : "#f0f0f0", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "marker1", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "75%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#001030", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FAFAFF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "marker2", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "75%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "40%", "stop-color" : "#001030", appendTo : gradient});
    $C({element : "stop", offset : "66%", "stop-color" : "#FAFAFF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "marker3", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "75%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#003090", appendTo : gradient});
    $C({element : "stop", offset : "94%", "stop-color" : "#E0F0FF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "LonGutter", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#00040A", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#00040A", "stop-opacity" :  0, appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "LatGutter", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#00040A", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#00040A", "stop-opacity" :  0, appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "paddleGrad", cx : "50%", cy : "50%", r : "69%", fx :  "50%", fy :  "50%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "55%", "stop-color" : "#F02424", appendTo : gradient});
    $C({element : "stop", offset : "78%", "stop-color" : "#F06060", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FF1010", appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "pushpinGrad_red", cx : "50%", cy : "50%", r : "69%", fx : "50%", fy : "50%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "55%", "stop-color" : "#F02424", appendTo : gradient});
    $C({element : "stop", offset : "78%", "stop-color" : "#F06060", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#FF1010", appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "pushpinGrad_green", cx : "50%", cy : "50%", r : "69%", fx : "50%", fy : "50%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "55%", "stop-color" : "#00C000", appendTo : gradient});
    $C({element : "stop", offset : "78%", "stop-color" : "#20C020", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#00C000", appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "pushpinGrad_blue", cx : "50%", cy : "50%", r : "69%", fx : "50%", fy : "50%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "55%", "stop-color" : "#4040FF", appendTo : gradient});
    $C({element : "stop", offset : "78%", "stop-color" : "#6868FF", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#4040FF", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "chiaroscuro", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "25%", "stop-color" : "#FFFFFF", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#A8A8A8", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "chrome", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "0%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "20%", "stop-color" : "#787878", appendTo : gradient});
    $C({element : "stop", offset : "32%", "stop-color" : "#FFFFFF", appendTo : gradient});
    $C({element : "stop", offset : "58%", "stop-color" : "#FFFFFF", appendTo : gradient});
    $C({element : "stop", offset : "58%", "stop-color" : "#686868", appendTo : gradient});
    $C({element : "stop", offset : "78%", "stop-color" : "#686868", appendTo : gradient});
    $C({element : "stop", offset : "96%", "stop-color" : "#808080", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "paddleReflection", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "16%", "stop-color" : "#FFC0C0", appendTo : gradient});
    $C({element : "stop", offset : "32%", "stop-color" : "#F06060", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "pinReflection_red", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "16%", "stop-color" : "#FFC0C0", appendTo : gradient});
    $C({element : "stop", offset : "32%", "stop-color" : "#F06060", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "pinReflection_green", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "16%", "stop-color" : "#C0FFC0", appendTo : gradient});
    $C({element : "stop", offset : "32%", "stop-color" : "#20CE20", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "pinReflection_blue", x1 : "0%", y1 : "0%", x2 : "100%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "16%", "stop-color" : "#C8C8FF", appendTo : gradient});
    $C({element : "stop", offset : "32%", "stop-color" : "#6868FF", appendTo : gradient});

    gradient = $C({element : "radialGradient", id : "leg_diskGrad", cx : "50%", cy : "50%", r : "69%", fx :  "50%", fy :  "50%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "45%", "stop-color" : "#1050AF", appendTo : gradient});
    $C({element : "stop", offset : "84%", "stop-color" : "#B0D0F0", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#0000E0", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "D3ButtonMaskGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#5C5C5C", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#282828", appendTo : gradient});

    gradient = $C({element : "linearGradient", id : "D3ButtonGrad", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : defs});
    $C({element : "stop", offset : "0%", "stop-color" : "#4682B4", appendTo : gradient});
    $C({element : "stop", offset : "100%", "stop-color" : "#3670A4", appendTo : gradient});
  }

};


pergola.customColorNames = {
  nürburgring : "#B8CAD2",
  city : "#CCD0D4",
  rustica : "#7B961E",
  suburban : "#DDFF58",
  frostica : "#F8FDFE",
  tropica : "#FF4200",
  herman : "#5E0200",
  glockenspiel : "#EDF4F4",
  black_diamond : "#293032",
  lightslategray : "rgb(144, 160, 176)"
};



pergola.skins[skin]();



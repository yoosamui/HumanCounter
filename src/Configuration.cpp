/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#include "Configuration.h"

void Configuration::load(string file)
{
    if (!XML.loadFile(file)) {

        settings.camviewmode = defaultSettings.camviewmode;
        settings.camvideofile = defaultSettings.camvideofile;
        settings.imagelightenAmount = defaultSettings.imagelightenAmount;
        settings.moghistory = defaultSettings.moghistory;
        settings.mogthreshold = defaultSettings.mogthreshold;
        settings.imagethreshold = defaultSettings.imagethreshold;
        settings.imageblur = defaultSettings.imageblur;
        settings.contourminarearadius = defaultSettings.contourminarearadius;
        settings.contourmaxarearadius = defaultSettings.contourmaxarearadius;
        settings.contourthreshold = defaultSettings.contourthreshold;
        settings.minblobsize = defaultSettings.minblobsize;
        settings.midblobsize = defaultSettings.midblobsize;
        settings.maxblobsize = defaultSettings.maxblobsize;
        settings.trackerpersistance = defaultSettings.trackerpersistance;
        settings.trackermaxDistance = defaultSettings.trackermaxDistance;
        settings.camcontrast = defaultSettings.camcontrast;
        settings.cambrightness = defaultSettings.cambrightness;
        settings.camrotation = defaultSettings.camrotation;
        settings.humandetection = defaultSettings.humandetection;
        settings.preset = defaultSettings.preset;
        settings.multithread = defaultSettings.multithread;

      
        cout << "Configuration File not found. Set default settings." << endl;
        return;
    }

    settings.camviewmode = XML.getValue(CONFIG_CAMVIEWMODE, defaultSettings.camviewmode);
    settings.camvideofile = XML.getValue(CONFIG_CAMVIDEOFILE, defaultSettings.camvideofile);
    settings.humandetection = XML.getValue(CONFIG_HUMANDETECTION, defaultSettings.humandetection);
    settings.preset = XML.getValue(CONFIG_PRESET, defaultSettings.preset);
    settings.multithread = XML.getValue(CONFIG_MULTITHREAD, defaultSettings.multithread);
    
    settings.imagelightenAmount = XML.getValue(CONFIG_LIGHTENAMOUNT, defaultSettings.imagelightenAmount);
    settings.moghistory = XML.getValue(CONFIG_MOGHISTORY, defaultSettings.moghistory);
    settings.mogthreshold = XML.getValue(CONFIG_MOGTHRESHOLD, defaultSettings.mogthreshold);
    settings.imagethreshold = XML.getValue(CONFIG_IMAGETHRESHOLD, defaultSettings.imagethreshold);
    settings.imageblur = XML.getValue(CONFIG_IMAGEBLUR, defaultSettings.imageblur);
    settings.contourminarearadius = XML.getValue(CONFIG_CONTOURMINAREARADIUS, defaultSettings.contourminarearadius);
    settings.contourmaxarearadius = XML.getValue(CONFIG_CONTOURMAXAREARADIUS, defaultSettings.contourmaxarearadius);
    settings.contourthreshold = XML.getValue(CONFIG_CONTOURTHRESHOLD, defaultSettings.contourthreshold);
    
    settings.minblobsize = XML.getValue(CONFIG_MINBLOBSIZE, defaultSettings.minblobsize);
    settings.midblobsize = XML.getValue(CONFIG_MIDBLOBSIZE, defaultSettings.midblobsize);
    settings.maxblobsize = XML.getValue(CONFIG_MAXBLOBSIZE, defaultSettings.maxblobsize);
    
    settings.trackerpersistance = XML.getValue(CONFIG_TRACKERPERSISTANCE, defaultSettings.trackerpersistance);
    settings.trackermaxDistance = XML.getValue(CONFIG_TRACKERMAXDISTANCE, defaultSettings.trackermaxDistance);
    settings.camcontrast = XML.getValue(CONFIG_CAMCONTRAST, defaultSettings.camcontrast);
    settings.cambrightness = XML.getValue(CONFIG_CAMBRIGHTNESS, defaultSettings.cambrightness);
    settings.camrotation = XML.getValue(CONFIG_CAMROTATION, defaultSettings.camrotation);

    
    XML.pushTag(CONFIG_MASKPOINTS);
    XML.pushTag(CONFIG_MASKPOINTS_POINT);

    int nbPoints = XML.getNumTags("point");
    for (int i = 0; i < nbPoints; i++) {
        int x, y;
        x = XML.getAttribute("point", "x", 0, i);
        y = XML.getAttribute("point", "y", 0, i);

        maskPoints.push_back(cv::Point(x, y));
    }

    XML.popTag();
    XML.popTag();

    cout << "Configuration loaded." << endl;

}

void Configuration::save(string file)
{

    ofxXmlSettings xml;

    xml.addTag("config");
    xml.pushTag("config");

    xml.setValue("camviewmode", settings.camviewmode);
    xml.setValue("camvideofile", settings.camvideofile);
    xml.setValue("humandetection", settings.humandetection);
    xml.setValue("preset", settings.preset);
    xml.setValue("multithread", settings.multithread);
    
    xml.setValue("imagelightenAmount", settings.imagelightenAmount);
    xml.setValue("imagethreshold", settings.imagethreshold);
    xml.setValue("imageblur", settings.imageblur);
    xml.setValue("moghistory", settings.moghistory);
    xml.setValue("mogthreshold", settings.mogthreshold);
    xml.setValue("minblobsize", settings.minblobsize);
    xml.setValue("midblobsize", settings.midblobsize);
    xml.setValue("maxblobsize", settings.maxblobsize);
    xml.setValue("contourminarearadius", settings.contourminarearadius);
    xml.setValue("contourmaxarearadius", settings.contourmaxarearadius);
    xml.setValue("contourthreshold", settings.contourthreshold);
    xml.setValue("trackerpersistance", settings.trackerpersistance);
    xml.setValue("trackermaxDistance", settings.trackermaxDistance);
    xml.setValue("camcontrast", settings.camcontrast);
    xml.setValue("cambrightness", settings.cambrightness);
    xml.setValue("camrotation", settings.camrotation);

    const string tag = "point";
    const string atrx = "x";
    const string atry = "y";

    for (int i = 0; i < maskPoints.size(); i++) {
        cv::Point point = maskPoints[i];
        if (i == 0) {
            xml.addTag(CONFIG_MASKPOINTS_POINT);
            xml.pushTag(CONFIG_MASKPOINTS_POINT, 0);

            xml.setAttribute(tag, atrx, point.x, 0);
            xml.setAttribute(tag, atry, point.y, 0);

            continue;
        }

        xml.addTag("point");
        xml.setAttribute(tag, atrx, point.x, i);
        xml.setAttribute(tag, atry, point.y, i);
        xml.pushTag("point", i);
        xml.popTag(); //pop position
    }

    xml.popTag(); //pop position
    xml.saveFile(CONFIGURATION_FILENAME);



}

Settings Configuration::getSettings()
{
    return settings;
}

void Configuration::setCamViewMode(int value)
{
    settings.camviewmode = value;
}

void Configuration::setImageLightenAmount(int value)
{
    settings.imagelightenAmount = value;
}

void Configuration::setImagethreshold(int value)
{
    settings.imagethreshold = value;
}

void Configuration::setImageBlur(int value)
{
    settings.imageblur = value;
}

void Configuration::setContourMinAreaRadius(int value)
{
    settings.contourminarearadius = value;
}

void Configuration::setContourMaxAreaRadius(int value)
{
    settings.contourmaxarearadius = value;
}

void Configuration::setContourThreshold(int value)
{
    settings.contourthreshold = value;
}
void Configuration::setMinBlobSize(int value)
{
    settings.minblobsize = value;
}

void Configuration::setMidBlobSize(int value)
{
    settings.midblobsize = value;
}

void Configuration::setMaxBlobSize(int value)
{
    settings.maxblobsize = value;
}

void Configuration::setTrackerMaxDistance(int value)
{
    settings.trackermaxDistance = value;
}
void Configuration::setTrackerPersistance(int value)
{
    settings.trackerpersistance = value;
}

void Configuration::setCamContrast(int value)
{
    settings.camcontrast = value;
}
void Configuration::setCamBrightness(int value)
{
    settings.cambrightness = value;
}

void Configuration::setCamRotation(int value)
{
    settings.camrotation = value;
}

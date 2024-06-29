#include "ImageViewer.hpp"

// -----add by NoRi 2024-06-24 -----------------------------------------------
#include <ESP32-targz.h>
#include <M5StackUpdater.h>
#define APP_VER   "SDU-imageViewer-v201-230624"
#define APP_NAME  "SDU-imageViewer"  // app Name
#define APP_BIN   "/21_imgView.bin"  // app bin file-name
#define TIMEOUT00 5000               // lobby countdouwn timeout : msec
#define D1_SERI   1
#define D2_DISP   2
#define D3_BOTH   3
// int SDCARD_CS_PIN = 4;
void setupSDUpdater(const char* appName);
bool SdBegin();
void prt(String sData, int direction = D3_BOTH);
void prtln(String sData, int direction = D3_BOTH);
void FOREVER_LOOP();
void POWER_OFF();
void REBOOT();
// bool AUTOMODE_ST;
// ----------------------------------------------------------------------------

#if defined(ARDUINO_M5STACK_DIAL) || defined(ARDUINO_M5STACK_DIN_METER)
#include "M5Encoder.hpp"

#if defined(ARDUINO_M5STACK_DIAL)
inline int16_t getEncoderOffset(void) {
    return 4;
}

inline int32_t getTextAreaX(void) {
    return 35;
}

inline int32_t getTextAreaY(void) {
    return 35;
}

inline int32_t getTextAreaWidth(void) {
    return 170;
}

inline int32_t getTextAreaHeight(void) {
    return 170;
}
#else
inline int16_t getEncoderOffset(void) {
    return 2;
}

inline int32_t getTextAreaX(void) {
    return 0;
}

inline int32_t getTextAreaY(void) {
    return 0;
}

inline int32_t getTextAreaWidth(void) {
    return M5.Lcd.width();
}

inline int32_t getTextAreaHeight(void) {
    return M5.Lcd.height();
}
#endif

static M5Encoder encoder;
static int16_t prev_dial_pos = 0;

inline void M5_BEGIN(m5::M5Unified::config_t cfg) {
    M5.begin(cfg);
    encoder.begin();
}

inline void M5_BEGIN(void) {
    auto cfg = M5.config();
    M5_BEGIN(cfg);
}

inline void M5_UPDATE(void) {
    M5.update();
}

inline int16_t getDirection(void) {
    // const long pos = M5Dial.Encoder.read();
    const int16_t pos = encoder.read();
    M5_LOGV("Dial: %d -> %d", prev_dial_pos, pos);
    if (abs(prev_dial_pos - pos) >= getEncoderOffset()) {
        const int16_t direction = pos - prev_dial_pos > 0 ? 1 : -1;
        prev_dial_pos = pos;
        return direction;
    } else {
        return 0;
    }
}
#else
inline void M5_BEGIN(m5::M5Unified::config_t cfg) {
    M5.begin(cfg);
}

inline void M5_BEGIN(void) {
    auto cfg = M5.config();
    M5.begin(cfg);
}

inline void M5_UPDATE(void) {
    M5.update();
}

inline int32_t getDirection(void) {
    if (M5.BtnA.wasClicked()) {
        prtln("BtnA was Clicked!", D1_SERI);
        return -1;
    } else if (M5.BtnC.wasClicked()) {
        prtln("BtnC was Clicked!", D1_SERI);
        return 1;
    } else {
        return 0;
    }
}

inline int32_t getTextAreaX(void) {
    return 0;
}

inline int32_t getTextAreaY(void) {
    return 0;
}

inline int32_t getTextAreaWidth(void) {
    return M5.Lcd.width();
}

inline int32_t getTextAreaHeight(void) {
    return M5.Lcd.height();
}
#endif

#include <Arduino_JSON.h>
#include <string.h>

// -----mode by NoRi 2024-06-24 -----------------------------------------------
const char* ImageViewer::VERSION = "v105-mod-V202";
const char* ImageViewer::DEFAULT_CONFIG_NAME = "/app/imgView/imgView.json";
String ImageViewer::DATA_DIR("/Pictures");
const char* ImageViewer::KEY_DATA_DIR = "DataDir";
// ----------------------------------------------------------------------------
// const char* ImageViewer::VERSION = "v1.0.5";
// const char* ImageViewer::DEFAULT_CONFIG_NAME = "image-viewer.json";
const char* ImageViewer::KEY_AUTO_MODE = "AutoMode";
const char* ImageViewer::KEY_AUTO_MODE_INTERVAL = "AutoModeInterval";
const char* ImageViewer::KEY_AUTO_MODE_RANDOMIZED = "AutoModeRandomized";
const char* ImageViewer::KEY_AUTO_ROTATION = "AutoRotation";
const char* ImageViewer::KEY_ORIENTATION = "Orientation";
const float ImageViewer::GRAVITY_THRESHOLD = 0.9F;
// const String ImageViewer::ROOT_DIR("/");
// static const bool FORMAT_FS_IF_FAILED = true;
static const char* EXT_JPG = ".jpg";
static const char* EXT_JPEG = ".jpeg";
static const char* EXT_BMP = ".bmp";
static const char* EXT_PNG = ".png";

ImageViewer::ImageViewer(bool isAutoMode, uint32_t autoModeInterval,
                         bool isAutoModeRandomized, bool isAutoRotation)
    : _orientation(0),
      _isAutoMode(isAutoMode),
      _autoModeInterval(autoModeInterval),
      _isAutoModeRandomized(isAutoModeRandomized),
      _isAutoRotation(isAutoRotation),
      _imageFiles{""},
      _nImageFiles(0),
      _pos(0),
      _prevUpdate(0),
      _interval(autoModeInterval) {
    randomSeed(analogRead(0));
}

ImageViewer::~ImageViewer(void) {
}

bool ImageViewer::begin(int bgColor) {
    // M5_BEGIN();
    // -- SDUpdater Lobby Screen by NoRi ----------
    // SDCARD_CS_PIN = (int)M5.getPin(m5::sd_spi_cs);
    // setupSDUpdater(APP_NAME);
    // ---------------------------------------------

    this->_orientation = M5.Lcd.getRotation();
#if defined(ARDUINO_M5STACK_CARDPUTER)  // TODO: removed when M5GFX v0.1.16 is
                                        // released
    this->_orientation = 1;
#endif
    M5.Lcd.setRotation(this->_orientation);

#if defined(ARDUINO_M5STACK_COREINK) || defined(ARDUINO_M5STACK_PAPER)
    M5.Lcd.invertDisplay(false);
    M5.Lcd.setEpdMode(epd_mode_t::epd_quality);
#endif

    // M5.Lcd.setTextScroll(true);
    // M5.Lcd.setCursor(getTextAreaX(), getTextAreaY());
    // M5.Lcd.setScrollRect(getTextAreaX(), getTextAreaY(), getTextAreaWidth(),
    //                      getTextAreaHeight());

    // M5.Display.setFont(&fonts::DejaVu18);
    // M5.Display.setTextColor(WHITE, BLACK);
    // M5.Display.setTextDatum(0);
    // M5.Display.setCursor(0, 0);
    // M5.Display.fillScreen(TFT_BLACK);
    // M5.Display.setTextScroll(true);

    // ------ NoRi ----------------------------------
    // if (!IV_FS.begin(FORMAT_FS_IF_FAILED)) {
    //     M5.Lcd.println("Failed to mount File System");
    //     return false;
    // }
    if (!SdBegin()) {
        prtln("Failed to mount SD File System");
        return false;
    }
    // ----------------------------------------------------

    M5.Lcd.setFileStorage(IV_FS);

    String msg = "Image Viewer " + String(VERSION);
    prtln(msg);

    if (!parse()) {
        return false;
    }

    M5_UPDATE();

    prtln("Mode:");
    if (M5.BtnA.isPressed()) {
        this->_isAutoMode = true;  // overriding the setting
        prtln(" Auto(Forced)");
    } else {
        String msg = String(this->_isAutoMode ? " Auto" : " Manual");
        prtln(msg);
    }

    prtln("Rotation:");
    if (this->_isAutoRotation) {
        if (M5.Imu.isEnabled()) {
            prtln(" Auto");
            if (M5.getBoard() == m5::board_t::board_M5Stack ||
                M5.getBoard() == m5::board_t::board_M5StackCoreS3 ||
                M5.getBoard() == m5::board_t::board_M5StackCore2) {
                M5.Imu.setAxisOrder(m5::IMU_Class::axis_y_pos,
                                    m5::IMU_Class::axis_x_neg,
                                    m5::IMU_Class::axis_z_pos);
            }
        } else {
            this->_isAutoRotation = false;
            prtln(" No(IMU disabled)");
        }
    } else {
        prtln(" No");
    }

    delay(DEFAULT_START_INTERVAL_MS);
    if (!setImageFileList()) {
        return false;
    }

    M5.Lcd.clearScrollRect();
    M5.Lcd.setCursor(0, 0);

    delay(DEFAULT_START_INTERVAL_MS);
    M5.Lcd.clear();
    M5.Lcd.fillScreen(bgColor);
    if (this->_isAutoRotation) {
        updateOrientation();
    } else {
        M5.Lcd.setRotation(this->_orientation);
    }

    if (!this->_isAutoMode) {
        showImage(this->_imageFiles, this->_pos);
    }

    return true;
}

bool ImageViewer::update(void) {
    // M5_UPDATE();

    if (this->_isAutoRotation && updateOrientation(GRAVITY_THRESHOLD)) {
        showImage(this->_imageFiles, this->_pos);
    }

    const uint32_t t = millis();
    int32_t direction = getDirection();
    if (direction == 0 && this->_isAutoMode &&
        t - this->_prevUpdate >= this->_interval) {
        direction = 1;
    }

    if (direction != 0) {
        this->_prevUpdate = t;
        if (direction < 0 && this->_pos == 0) {
            this->_pos = this->_nImageFiles - 1;
        } else if (direction > 0 && this->_pos == this->_nImageFiles - 1) {
            this->_pos = 0;
        } else {
            this->_pos += direction;
        }
        showImage(this->_imageFiles, this->_pos);
        if (this->_isAutoMode && this->_isAutoModeRandomized) {
            this->_interval = random(this->_autoModeInterval);
        }
    }
    return direction != 0;
}

bool ImageViewer::setImageFileList(const String& path) {
    File root = IV_FS.open(path.c_str(), "r");
    if (!root and !root.isDirectory()) {
        String msg = "Failed to open \"" + DATA_DIR + "\"";
        prtln(msg);
        return false;
    } else {
        String msg =
            "Success to open image data folder :  \"" + DATA_DIR + "\"";
        prtln(msg);
    }

    File f = root.openNextFile();
    while (f && this->_nImageFiles < MAX_IMAGE_FILES) {
        if (!f.isDirectory() && isImageFile(f)) {
            // ---- mod by NoRi ---
            // this->_imageFiles[this->_nImageFiles] = DATA_DIR + f.name();
            this->_imageFiles[this->_nImageFiles] = DATA_DIR + "/" + f.name();
            // -----------------------------------------------------------------
            ++this->_nImageFiles;
        }
        f = root.openNextFile();
    }

    if (this->_nImageFiles == 0) {
        prtln("No image files found");
        return false;
    } else {
        //  --- sort -------------------------------------------
        size_t N = this->_nImageFiles;
        for (int i = 0; i < N - 1; ++i) {
            for (int j = 0; j < N - 1; ++j) {
                if (this->_imageFiles[j] > this->_imageFiles[j + 1]) {
                    // swap
                    String tmp = this->_imageFiles[j];
                    this->_imageFiles[j] = this->_imageFiles[j + 1];
                    this->_imageFiles[j + 1] = tmp;
                }
            }
        }
    }

    prtln("Image Files:");
    for (size_t c = 0; c < this->_nImageFiles; ++c) {
        String msg = " " + String(this->_imageFiles[c]);
        prtln(msg);
        delay(FILE_LIST_DISPLAY_INTERVAL_MS);
    }
    return true;
}

bool ImageViewer::updateOrientation(float threshold) {
    const uint8_t o = detectOrientation(threshold);
    if (this->_orientation != o) {
        M5_LOGD("Change Orientation: %d -> %d", this->_orientation, o);
        this->_orientation = o;
        M5.Lcd.setRotation(this->_orientation);
        return true;
    }
    return false;
}

bool ImageViewer::isAutoMode() {
    return _isAutoMode;
}

void ImageViewer::setAutoMode(bool mode) {
    _isAutoMode = mode;
}

void ImageViewer::showImage(const String images[], size_t p) {
    const char* filename = images[p].c_str();
    String msg = "showImage FileName = " + String(filename);
    prtln(msg, D1_SERI);

    M5.Lcd.startWrite();
    if (isJpeg(filename)) {
        M5.Lcd.drawJpgFile(filename, 0, 0, M5.Display.width(),
                           M5.Display.height(), 0, 0, 0.0F, 0.0F,
                           middle_center);
    } else if (isPng(filename)) {
        M5.Lcd.drawPngFile(filename, 0, 0, M5.Display.width(),
                           M5.Display.height(), 0, 0, 0.0F, 0.0F,
                           middle_center);
    } else if (isBmp(filename)) {
        M5.Lcd.drawBmpFile(filename, 0, 0, M5.Display.width(),
                           M5.Display.height(), 0, 0, 0.0F, 0.0F,
                           middle_center);
    } else {
        // M5.Lcd.printf("ignore: %s", filename);
        // M5.Lcd.println();
        String msg2 = "ignore: " + String(filename);
        prtln(msg2);
    }
    M5.Lcd.endWrite();
}

bool ImageViewer::hasExt(const char* filename, const char* ext) const {
    if (filename == nullptr) {
        return false;
    }
    if (ext == nullptr) {
        return false;
    }
    const char* p = strrchr(filename, '.');
    return p != nullptr && strcasecmp(ext, p) == 0;
}

bool ImageViewer::isJpeg(const char* filename) const {
    if (filename == nullptr) {
        return false;
    }
    return hasExt(filename, EXT_JPG) || hasExt(filename, EXT_JPEG);
}

bool ImageViewer::isPng(const char* filename) const {
    if (filename == nullptr) {
        return false;
    }
    return hasExt(filename, EXT_PNG);
}

bool ImageViewer::isBmp(const char* filename) const {
    if (filename == nullptr) {
        return false;
    }
    return hasExt(filename, EXT_BMP);
}

bool ImageViewer::isImageFile(const File& f) const {
    const char* name = f.name();
    return isJpeg(name) || isPng(name) || isBmp(name);
}

uint8_t ImageViewer::detectOrientation(float threshold) {
    if (M5.Imu.isEnabled()) {
        float ax, ay, az;
        M5.Imu.getAccel(&ax, &ay, &az);
        M5_LOGV("Accel: ax: %f, ay: %f, az: %f", ax, ay, az);
        if (ay >= threshold) {
            return 0;
        } else if (ax >= threshold) {
            return 1;
        } else if (ax <= -threshold) {
            return 3;
        } else if (ay <= -threshold) {
            return 2;
        }
    }
    return 0;
}

bool ImageViewer::parse(const char* config) {
    if (config == nullptr) {
        prtln("config is null");
        return false;
    }
    const String filename = config;

    if (!IV_FS.exists(filename)) {
        String msg = filename + " is not found";
        prtln(msg);
        return true;  // use default
    }
    String msg = "Config: " + filename;
    prtln(msg);

    File f = IV_FS.open(filename, "r");
    if (!f) {
        String msg = " E: failed to open";
        prtln(msg);
        return false;
    }
    uint8_t buf[f.size()] = {0};
    f.read(buf, sizeof(buf));
    f.close();

    JSONVar o = JSON.parse((const char*)buf);
    if (JSON.typeof(o) == "undefined") {
        prtln(" E: parse");
        return false;
    }
    if (o.hasOwnProperty(KEY_AUTO_MODE)) {
        this->_isAutoMode = (bool)o[KEY_AUTO_MODE];
    }
    msg = " AutoMode: " + String(this->_isAutoMode ? "true" : "false");
    prtln(msg);

    if (o.hasOwnProperty(KEY_AUTO_MODE_INTERVAL)) {
        this->_autoModeInterval = (uint32_t)o[KEY_AUTO_MODE_INTERVAL];
        this->_interval = _autoModeInterval;
    }
    msg = " AutoModeInterval: " + String(this->_autoModeInterval, 10);
    prtln(msg);
    msg = "_Interval: " + String(this->_interval, 10);
    prtln(msg);

    if (o.hasOwnProperty(KEY_AUTO_MODE_RANDOMIZED)) {
        this->_isAutoModeRandomized = (bool)o[KEY_AUTO_MODE_RANDOMIZED];
    }
    msg = " Randomized: " +
          String(this->_isAutoModeRandomized ? "true" : "false");
    prtln(msg);

    if (o.hasOwnProperty(KEY_AUTO_ROTATION)) {
        this->_isAutoRotation = (bool)o[KEY_AUTO_ROTATION];
    }
    msg = " AutoRotation: " + String(this->_isAutoRotation ? "true" : "false");
    prtln(msg);

    if (o.hasOwnProperty(KEY_ORIENTATION)) {
        JSONVar orientationVar = o[KEY_ORIENTATION];
        if (JSON.typeof(orientationVar) == "number") {
            int orientationInt = (int)orientationVar;
            if (0 <= orientationInt && orientationInt <= 7) {
                this->_orientation = orientationInt;
            } else {
                this->_orientation = M5.Lcd.getRotation();
                M5_LOGE("Invalid Orientation Value: %d", orientationInt);
            }
        } else {
            this->_orientation = M5.Lcd.getRotation();
            M5_LOGE("Illegal Orientation Type: %s, Value: %s",
                    JSON.typeof(orientationVar).c_str(),
                    JSONVar::stringify(orientationVar).c_str());
        }
    } else {
        this->_orientation = M5.Lcd.getRotation();
        M5_LOGW("Default Orientation is not found");
    }
    M5.Lcd.printf(" Orientation: %s", getOrientationString(this->_orientation));
    M5.Lcd.println();

    // --- DATA_DIR ---
    if (o.hasOwnProperty(KEY_DATA_DIR)) {
        String getStr1 = JSON.stringify(o[KEY_DATA_DIR]);
        int len = getStr1.length();
        String getStr2 = getStr1.substring(1, len - 1);
        DATA_DIR = getStr2;
        prtln("DATA_DIR = " + DATA_DIR);
    }

    return true;
}

// -----add by NoRi 2024-06-24 -----------------------------------------------
// #define MDM2   -2    // init
// #define MDM1   -1    // setup start
// #define MD00   0     // setup don (Normal mode)
// #define MD01   1     // MD01  FileSystem change
// #define MD02   2     // MD02  load menu.bin
// #define MD03   3     // MD03  save bin to SD
// #define MD04   4     // MD04  power off
// #define MD_END 4     //
// extern int MODE_ST;  // mode status

bool SdBegin() {
    // --- SD begin -------
    int i = 0;
    bool success = false;
    prtln("SD.begin Start", D1_SERI);

    while (i < 3) {  // SDカードマウント待ち
        success = SD.begin((int)M5.getPin(m5::sd_spi_cs), SPI, 4000000U, "/sd",
                           10U, false);
        if (success)
            return true;

        prtln("SD Wait...", D1_SERI);
        delay(500);
        i++;
    }

    if (i >= 3) {
        prtln("SD.begin faile", D3_BOTH);
        return false;
    } else
        return true;
}

void prt(String sData, int direction) {
    switch (direction) {
        case D3_BOTH:
            M5.Log.print(sData.c_str());
            M5.Display.print(sData.c_str());
            break;

        case D2_DISP:
            M5.Display.print(sData.c_str());
            break;

        case D1_SERI:
            M5.Log.print(sData.c_str());
            break;

        default:
            break;
    }
}

void prtln(String sData, int direction) {
    String strData = sData + "\n";
    prt(strData, direction);
}


void sdu_lobby();

void sdu_lobby()
{
    setupSDUpdater(APP_NAME);
}

void setupSDUpdater(const char* appName) {
    SDUCfg.setLabelMenu("< Menu");  // load menu.bin
    SDUCfg.setLabelSkip("Skip");    // skip the lobby countdown and run the app
    SDUCfg.setAppName(appName);     // lobby screen label: application name
    checkSDUpdater(SD,              // filesystem (default=SD)
                   MENU_BIN,        // path to binary (default=/menu.bin, empty
                                    // string=rollback only)
                   TIMEOUT00,  // wait delay, (default=0, will be forced to 2000
                               // upon ESP.restart() )
                   (int)M5.getPin(m5::sd_spi_cs));
}

void FOREVER_LOOP() {
    while (true) {
        delay(10);
    }
}

void POWER_OFF() {
    prtln("\n\n*** POWER_OFF ***", D1_SERI);
    //   SPIFFS.end();
    SD.end();

    delay(3000);
    M5.Power.powerOff();
    FOREVER_LOOP();
}

void REBOOT() {
    prtln("\n\n*** REBOOT ***", D3_BOTH);
    //   SPIFFS.end();
    SD.end();

    delay(3000);
    ESP.restart();
    FOREVER_LOOP();
}

void loadMenu(void) {
    updateFromFS(SD);
}

void saveBin(void) {
    saveSketchToFS(SD, APP_BIN);
}

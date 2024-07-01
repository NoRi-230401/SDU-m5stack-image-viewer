#ifndef _IMAGE_VIWER_HPP_
#define _IMAGE_VIWER_HPP_

#include <SD.h>
#define IV_FS SD
#include <M5Unified.h>

#define AUTOMODE_OFF 0
#define AUTOMODE_FORWARD 1
#define AUTOMODE_BACKRWARD 2
#define AUTOMODE_RND 3

class ImageViewer
{
public:
    inline const char *getOrientationString(uint8_t rotation)
    {
        switch (rotation)
        {
        case 0:
            return "CW_0";
        case 1:
            return "CW_90";
        case 2:
            return "CW_180";
        case 3:
            return "CW_270";
        case 4:
            return "CCW_0";
        case 5:
            return "CCW_90";
        case 6:
            return "CCW_180";
        case 7:
            return "CCW_270";
        default:
            return "Unknown";
        }
    }

    static const char *VERSION;
    static const char *DEFAULT_CONFIG_NAME;
    static const char *KEY_AUTO_MODE;
    static const char *KEY_AUTO_MODE_INTERVAL;
    static const char *KEY_AUTO_MODE_RANDOMIZED;
    static const char *KEY_AUTO_ROTATION;
    static const char *KEY_ORIENTATION;
    static const char *KEY_DATA_DIR;

    static const size_t MAX_IMAGE_FILES = 50;
    static const u_int8_t DEFAULT_AUTO_MODE = AUTOMODE_OFF;
    static const uint32_t DEFAULT_START_INTERVAL_MS = 3000;
    static const uint32_t DEFAULT_AUTO_MODE_INTERVAL_MS = 3000;
    static const bool DEFAULT_AUTO_MODE_RANDOMIZED = false;
    static const bool DEFAULT_AUTO_ROTATION = true;
    static const uint32_t FILE_LIST_DISPLAY_INTERVAL_MS = 100;

    static const float GRAVITY_THRESHOLD;
    static String DATA_DIR;

    ImageViewer(uint8_t isAutoMode = DEFAULT_AUTO_MODE,
                uint32_t autoModeInterval = DEFAULT_AUTO_MODE_INTERVAL_MS,
                bool isAutoModeRandomize = DEFAULT_AUTO_MODE_RANDOMIZED,
                bool isAutoRotation = DEFAULT_AUTO_ROTATION);
    virtual ~ImageViewer(void);

    virtual bool begin(const int bgColor = TFT_WHITE);
    virtual bool update(void);

    virtual bool updateOrientation(float threshold = GRAVITY_THRESHOLD);

    // ------------ Add by NoRi ------
    // virtual bool isAutoMode();
    // virtual void setAutoMode(bool mode);
    virtual uint8_t getAutoMode();
    virtual void setAutoMode(uint8_t mode);

protected:
    virtual bool setImageFileList(const String &path = DATA_DIR);
    virtual void showImage(const String images[], size_t p);
    virtual bool hasExt(const char *filename, const char *ext) const;
    virtual bool isJpeg(const char *filename) const;
    virtual bool isPng(const char *filename) const;
    virtual bool isBmp(const char *filename) const;
    virtual bool isImageFile(const File &f) const;
    virtual uint8_t detectOrientation(float threshold);
    virtual bool parse(const char *config = DEFAULT_CONFIG_NAME);

private:
    uint8_t _orientation;
    // bool _isAutoMode;
    uint8_t _isAutoMode;

    uint32_t _autoModeInterval;
    bool _isAutoModeRandomized;
    bool _isAutoRotation;

    String _imageFiles[MAX_IMAGE_FILES];
    size_t _nImageFiles;
    size_t _pos;
    uint32_t _prevUpdate;
    uint32_t _interval;
};

// ----------------------------------------------------------------------------
#endif //_IMAGE_VIEWER_HPP_

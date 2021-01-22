#ifndef LTDMDLLWRAPPER_H
#define LTDMDLLWRAPPER_H

#include <QLibrary>

#ifdef WIN32
#pragma pack(push)
#pragma pack(4)
#endif

   typedef struct _DDCFile            DDCFile;
   typedef struct _DDCChannelGroup      DDCChannelGroup;
   typedef struct _DDCChannel         DDCChannel;

   typedef DDCFile*               DDCFileHandle;
   typedef DDCChannelGroup*         DDCChannelGroupHandle;
   typedef DDCChannel*               DDCChannelHandle;

#ifdef WIN32
#pragma pack(pop)
#endif

typedef enum {
    DDC_UInt8 = 5,   // unsigned char
    DDC_Int16 = 2,   // short
    DDC_Int32 = 3,   // int
    DDC_Float = 9,   // float
    DDC_Double = 10,   // double
    DDC_String = 23,   // string
    DDC_Timestamp = 30,   // timestamp (Year/Month/Day/Hour/Minute/Second/Millisecond components)
} DDCDataType;

// File type constants
#define DDC_FILE_TYPE_TDM            "TDM"
#define DDC_FILE_TYPE_TDM_STREAMING  "TDMS"

// File property constants
#define DDC_FILE_NAME               "name"            // Name
#define DDC_FILE_DESCRIPTION        "description"      // Description
#define DDC_FILE_TITLE              "title"            // Title
#define DDC_FILE_AUTHOR             "author"         // Author
#define DDC_FILE_DATETIME           "datetime"         // Date/Time

// ChannelGroup property constants
#define DDC_CHANNELGROUP_NAME         "name"            // Name
#define DDC_CHANNELGROUP_DESCRIPTION  "description"      // Description

// Channel property constants
#define DDC_CHANNEL_NAME            "name"            // Name
#define DDC_CHANNEL_DESCRIPTION     "description"      // Description
#define DDC_CHANNEL_UNIT_STRING     "unit_string"      // Unit String
#define DDC_CHANNEL_MINIMUM         "minimum"         // Minimum
#define DDC_CHANNEL_MAXIMUM         "maximum"         // Maximum

// Error codes
typedef enum {

    DDC_NoError = 0,         // No error
    DDC_ErrorBegin = -6201,      //

    DDC_OutOfMemory = -6201,      // The library could not allocate memory.
    DDC_InvalidArgument = -6202,      // An invalid argument was passed to the library.
    DDC_InvalidDataType = -6203,      // An invalid data type was passed to the library.
    DDC_UnexpectedError = -6204,      // An unexpected error occurred in the library.
    DDC_UsiCouldNotBeLoaded = -6205,      // The USI engine could not be loaded.
    DDC_InvalidFileHandle = -6206,      // An invalid file handle was passed to the library.
    DDC_InvalidChannelGroupHandle = -6207,      // An invalid channel group handle was passed to the library.
    DDC_InvalidChannelHandle = -6208,      // An invalid channel handle was passed to the library.
    DDC_FileDoesNotExist = -6209,      // The file passed to the library does not exist.
    DDC_CannotWriteToReadOnlyFile = -6210,      // The file passed to the library is read only and cannot be modified.
    DDC_StorageCouldNotBeOpened = -6211,      // The storage could not be opened.
    DDC_FileAlreadyExists = -6212,      // The file passed to the library already exists and cannot be created.
    DDC_PropertyDoesNotExist = -6213,      // The property passed to the library does not exist.
    DDC_PropertyDoesNotContainData = -6214,      // The property passed to the library does not have a value.
    DDC_PropertyIsNotAScalar = -6215,      // The value of the property passed to the library is an array and not a scalar.
    DDC_DataObjectTypeNotFound = -6216,      // The object type passed to the library does not exist.
    DDC_NotImplemented = -6217,      // The current implementation does not support this operation.
    DDC_CouldNotSaveFile = -6218,      // The file could not be saved.
    DDC_MaximumNumberOfDataValuesExceeded = -6219,      // The request would exceed the maximum number of data values for a channel.
    DDC_InvalidChannelName = -6220,      // An invalid channel name was passed to the library.
    DDC_DuplicateChannelName = -6221,      // The channel group already contains a channel with this name.
    DDC_DataTypeNotSupported = -6222,      // The current implementation does not support this data type.
    DDC_FileAccessDenied = -6224,      // File access denied.
    DDC_InvalidTimeValue = -6225,      // The specified time value is invalid.
    DDC_ReplaceNotSupportedForSavedTDMSData = -6226,      // The replace operation is not supported on data that has already been saved to a TDM Streaming file.
    DDC_PropertyDataTypeMismatch = -6227,      // The data type of the property does not match the expected data type.
    DDC_ChannelDataTypeMismatch = -6228,      // The data type of the channel does not match the expected data type.

    DDC_ErrorEnd = -6228,        //
    DDC_ErrorForceSizeTo32Bits = 0xffffffff   //

} DDCError;

typedef int (__stdcall DDC_CreateFile)(const char *filePath,
                                           const char *fileType,
                                           const char *name,
                                           const char *description,
                                           const char *title,
                                           const char *author,
                                           DDCFileHandle *file);

typedef int (__stdcall DDC_AddChannelGroup)(DDCFileHandle file,
   const char *name,
   const char *description,
   DDCChannelGroupHandle *channelGroup);

typedef int (__stdcall DDC_AddChannel)(DDCChannelGroupHandle channelGroup,
   DDCDataType dataType,
   const char *name,
   const char *description,
   const char *unitString,
   DDCChannelHandle *channel);

typedef int (__stdcall DDC_SaveFile)(DDCFileHandle file);

typedef int (__stdcall DDC_CloseFile)(DDCFileHandle file);

typedef int (__stdcall DDC_RemoveChannelGroup)(DDCChannelGroupHandle channelGroup);

typedef int (__stdcall DDC_RemoveChannel)(DDCChannelHandle channel);

typedef int (__stdcall DDC_CloseChannelGroup)(DDCChannelGroupHandle channelGroup);

typedef int (__stdcall DDC_CloseChannel)(DDCChannelHandle channel);

typedef int (__stdcall DDC_SetDataValues)(DDCChannelHandle channel,
      void *values,
      size_t numValues);

typedef int (__stdcall DDC_SetDataValuesTimestampComponents)(DDCChannelHandle channel,
      unsigned int year[],
      unsigned int month[],
      unsigned int day[],
      unsigned int hour[],
      unsigned int minute[],
      unsigned int second[],
      double milliSecond[],
      size_t numValues);

typedef int (__stdcall DDC_AppendDataValues)(DDCChannelHandle channel,
      void *values,
      size_t numValues);

typedef int (__stdcall DDC_AppendDataValuesTimestampComponents)(DDCChannelHandle channel,
      unsigned int year[],
      unsigned int month[],
      unsigned int day[],
      unsigned int hour[],
      unsigned int minute[],
      unsigned int second[],
      double milliSecond[],
      size_t numValues);

typedef int (__stdcall DDC_GetChannels)(DDCChannelGroupHandle channelGroup,
   DDCChannelHandle channelsBuf[],
   size_t numChannels);

typedef int (__stdcall DDC_OpenFileEx)(const char *filePath,
                                       const char *fileType,
                                       int readOnly,
                                       DDCFileHandle *file);

typedef int (__stdcall DDC_GetNumChannelGroups)(DDCFileHandle file,
                                                unsigned int *numberOfChannelGroups);

typedef int (__stdcall DDC_GetChannelGroups)(DDCFileHandle file,
                                             DDCChannelGroupHandle channelGroupsBuffer[],
                                             size_t numberOfChannelGroups);

typedef int (__stdcall DDC_GetChannelGroupStringPropertyLength)(DDCChannelGroupHandle channelGroup,
                                                                const char *property,
                                                                unsigned int *length);

typedef int (__stdcall DDC_GetChannelGroupProperty)(DDCChannelGroupHandle channelGroup,
                                                    const char *property,
                                                    void *value,
                                                    size_t valueSizeInBytes);

typedef int (__stdcall DDC_GetNumChannels)(DDCChannelGroupHandle channelGroup,
                                           unsigned int *numberOfChannels);

typedef int (__stdcall DDC_GetNumDataValues)(DDCChannelHandle channel,
                                             unsigned __int64 *numberOfValues);

typedef int (__stdcall DDC_GetDataValues)(DDCChannelHandle channel,
                                          size_t indexOfFirstValueToGet,
                                          size_t numberOfValuesToGet,
                                          void *values);

typedef int (__stdcall DDC_GetDataValuesDouble)(DDCChannelHandle channel,
                                                size_t indexOfFirstValueToGet,
                                                size_t numValuesToGet,
                                                double values[]);

typedef int (__stdcall DDC_GetDataValuesTimestampComponents)(DDCChannelHandle channel,
                                                             size_t indexOfFirstValueToGet,
                                                             size_t numValuesToGet, unsigned int year[],
                                                             unsigned int month[],
                                                             unsigned int day[],
                                                             unsigned int hour[],
                                                             unsigned int minute[],
                                                             unsigned int second[],
                                                             double milliSecond[],
                                                             unsigned int weekDay[]);

typedef void (__stdcall DDC_FreeMemory)(void *memoryPointer);

class LTdmDllWrapper
{
public:
    static LTdmDllWrapper* instance()
    {
        // Do we have an object yet?
        if(s_pInstance == nullptr) {
          // No, create it
          s_pInstance = new LTdmDllWrapper();
        }
        // Return the instance ptr
        return s_pInstance;
    }

    static void destroy()
    {
        // Clean up the pointer
        if(s_pInstance != nullptr) {
            delete s_pInstance, s_pInstance = NULL;
        }
    }

    DDC_CreateFile*                             createFile;
    DDC_AddChannelGroup*                        addChannelGroup;
    DDC_AddChannel*                             addChannel;
    DDC_SaveFile*                               saveFile;
    DDC_CloseFile*                              closeFile;
    DDC_RemoveChannelGroup*                     removeChannelGroup;
    DDC_RemoveChannel*                          removeChannel;
    DDC_CloseChannelGroup*                      closeChannelGroup;
    DDC_CloseChannel*                           closeChannel;
    DDC_SetDataValues*                          setDataValues;
    DDC_SetDataValuesTimestampComponents*       setDataValuesTimestampComponents;
    DDC_AppendDataValues*                       appendDataValues;
    DDC_AppendDataValuesTimestampComponents*    appendDataValuesTimestampComponents;
    DDC_GetChannels*                            getChannels;
    DDC_OpenFileEx*                             openFileEx;
    DDC_GetNumChannelGroups*                    getNumChannelGroups;
    DDC_GetChannelGroups*                       getChannelGroups;
    DDC_GetChannelGroupStringPropertyLength*    getChannelGroupStringPropertyLength;
    DDC_GetChannelGroupProperty*                getChannelGroupProperty;
    DDC_GetNumChannels*                         getNumChannels;
    DDC_GetNumDataValues*                       getNumDataValues;
    DDC_GetDataValues*                          getDataValues;
    DDC_GetDataValuesDouble*                    getDataValuesDouble;
    DDC_GetDataValuesTimestampComponents*       getDataValuesTimestampComponents;
    DDC_FreeMemory*                             freeMemory;

private:
    LTdmDllWrapper();

private:
    static LTdmDllWrapper   *s_pInstance;       //!< instance ptr

    QLibrary m_library;
};

#endif // LTDMDLLWRAPPER_H

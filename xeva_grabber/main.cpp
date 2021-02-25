#include "stdio.h" // C Standard Input/Output library.
#include "XCamera.h" // Xeneth SDK main header.
#include "XFilters.h" // Xeneth SDK main header.

/* function declaration */
int grab_image(char CalPack[100], char OutFile[100]);



/*parent routine*/
int main(int argc, char** argv) {

    /* local variable definition */
    int ret;

    /* testing */
    //char CalPack[100] = "calib.xca";
    //char OutFile[100] = "b.xpng";
    //ret = grab_image(CalPack,OutFile);

    /* call function using external arguments */
    ret = grab_image(argv[2],argv[1]);

    return 0;
}

/*function to routine*/


int grab_image(char CalPack[100],char OutFile[100])
{

    // Variables
    XCHANDLE handle = 0;      // Handle to the camera
    ErrCode  errorCode = 0;   // Used to store returned errorCodes from the SDK functions.
    word*    frameBuffer = 0; // 16-bit buffer to store the capture frame.
    dword    frameSize = 0;   // The size in bytes of the raw image.

    // Open a connection to the first detected camera by using connection string cam://0
    handle = XC_OpenCamera("cam://0");

    // When the connection is initialised, ...
    if (XC_IsInitialised(handle))
    {
        // ... start capturing
        if ((errorCode = XC_StartCapture(handle)) != I_OK)
        {
            printf("Could not start capturing, errorCode: %lu\n", errorCode);
        }
        else if (XC_IsCapturing(handle)) // When the camera is capturing ...
        {

            // Calibration Pack
            XC_LoadCalibration(handle, CalPack, XLC_StartSoftwareCorrection);

            // Determine native framesize.
            frameSize = XC_GetFrameSize(handle);

            // Initialize the 16-bit buffer.
            frameBuffer = new word[frameSize / 2];

            // ... grab a frame from the camera.
            if ((errorCode = XC_GetFrame(handle, FT_NATIVE, XGF_Blocking, frameBuffer, frameSize)) != I_OK)
            {
                printf("Problem while fetching frame, errorCode %lu", errorCode);
            }

            errorCode = XC_SaveData(handle, OutFile, XSD_SaveThermalInfo | XSD_Force16);

        }
    }
    else
    {
        printf("Initialization failed\n");
    }

    // Cleanup.

    // When the camera is still capturing, ...
    if (XC_IsCapturing(handle))
    {
        // ... stop capturing.
        if ((errorCode = XC_StopCapture(handle)) != I_OK)
        {
            printf("Could not stop capturing, errorCode: %lu\n", errorCode);
        }
    }

    // When the handle to the camera is still initialised ...
    if (XC_IsInitialised(handle))
    {
        XC_CloseCamera(handle);
    }

    if (frameBuffer != 0)
    {
        delete[] frameBuffer;
        frameBuffer = 0;
    }

    return 0;
}

/* **************************************************************
** NAME:            Fast Bitmap Rotation and Scaling
** AUTHOR:          Steven M Mortimer
** COMPILER:        Visual C++ V6.0 Enterprise SP3
** Target Platform: Win32
** RIGHTS:          Stevem Mortimer
** Date:            18th Jan 2000
************************************************************** */
#include "def.h"
#include "rotate.h"
#include "cdib.h"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

double deg2rad(double degree)
{
    return degree * CV_PI / 180;
    }


void cout_indented(int n_space, const string& str)
{
    std::cout << std::string(n_space, ' ') << str << std::endl;
    }



string mat_type_2_str(int type, int n_sp) 
{
    cout_indented(n_sp, "mat_type_2_str");
        string r;
            uchar depth = type & CV_MAT_DEPTH_MASK;
                uchar chans = 1 + (type >> CV_CN_SHIFT);
                    switch ( depth ) {
                            case CV_8U:  r = "8U"; break;
                                    case CV_8S:  r = "8S"; break;
                                            case CV_16U: r = "16U"; break;
                                                    case CV_16S: r = "16S"; break;
                                                            case CV_32S: r = "32S"; break;
                                                                    case CV_32F: r = "32F"; break;
                                                                            case CV_64F: r = "64F"; break;
                                                                                    default:     r = "User"; break;
                                                                                        }
                                                                                            r += "C";
                                                                                                r += (chans + '0');
                                                                                                    return r;
                                                                                                    }


                                                                                                    //  Mat mat = Mat::zeros(100, 10, CV_8UC3);
                                                                                                    //  print_mat_type(mat, 0);
                                                                                                    //  => mat type : 8UC3    

                                                                                                    void print_mat_type(const Mat& mat, int n_sp)
                                                                                                    {
                                                                                                        cout_indented(n_sp, "print_mat_type");
                                                                                                            cout_indented(n_sp + 1, "mat type : " + mat_type_2_str(mat.type(), n_sp + 1));         
                                                                                                            }

#ifndef _WIN32


int main(int argc, char **argv)
{
	if(argc < 6) 
	{
		cout << "Usage : ImageRotation [file_name] [center_x_ratio] [center_y_ratio] [angle_deg]" << endl;	exit(0);
	}
    cout << "sizeof(WDIBPIXEL) : " << sizeof(WDIBPIXEL) << endl;    //exit(0); 
    float ratio_cx = atof(argv[2]), ratio_cy = atof(argv[3]), fAngleDeg = atof(argv[4]);  
    bool is_mosaicking = 0 != atoi(argv[5]);  
    Mat im_255_edge, im_gray = imread(argv[1], 0);
    print_mat_type(im_gray, 0);
    //Canny(im_gray, im_255_edge, 50, 150);
    im_255_edge = im_gray;//, im_255_edge, 50, 150);
    Mat im_255_edge_rotated = Mat::zeros(Size(im_255_edge.cols * 2, im_255_edge.rows * 2), im_255_edge.type()); 
    WDIBPIXEL *pDstBase = im_255_edge_rotated.data, *pSrcBase = im_255_edge.data;
    int srcW = im_255_edge.cols, srcH = im_255_edge.rows, srcDelta = im_255_edge.step;
    int dstW = srcW * 2, dstH = srcH * 2, dstDelta = srcDelta * 2;
    float fSrcCX = ratio_cx * srcW, fSrcCY = ratio_cy * srcH;
    float fDstCX = fSrcCX * 1.2, fDstCY = fSrcCY * 1.2, fAngle = deg2rad(fAngleDeg), fScale = 1.0;

    RotateWrapFill(
        pDstBase, dstW, dstH, dstDelta,
        pSrcBase, srcW, srcH, srcDelta,
        fDstCX, fDstCY,
        fSrcCX, fSrcCY,
        fAngle, fScale, 
        is_mosaicking, 1);
    
    imshow("im_255_edge", im_255_edge); imshow("im_255_edge_rotated", im_255_edge_rotated); waitKey();
    cout_indented(0, "WWW");
	//int libraryHD = atoi(argv[1]);
	
    
    return 1;
}
	
#else       // _WIN32

#include <windows.h>
#include <windowsx.h>

/////////////////////////////////////////////////////////////////
// defines
#define _MINSCALE   0.4f
#define _MAXSCALE   5.0f
#define SZIMAGE     "test1.bmp"
 
/////////////////////////////////////////////////////////////////
// Globals
CDIB*   gDibSrc         = NULL;
CDIB*   gDibDst         = NULL;
float   gdScale         = _MAXSCALE;
float   gdAngle         = 0.0f;
float   gdScaleDir      = 0.1f;
double  gdTicksPerSec   = 0.0;
bool    gbTimeFunction  = false;
 
 
/////////////////////////////////////////////////////////////////
// Function Prototypes
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
 
/////////////////////////////////////////////////////////////////
// WinMain
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR szCmdLine, int iCmdShow)
{
 
    HWND        hwnd;
    MSG         msg;    
 
    // Register the window class
    WNDCLASS wndclass;
    ZeroMemory(&wndclass, sizeof(WNDCLASS));
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = __FILE__;
    RegisterClass(&wndclass);
 
    // Check if we can use the high performace counter
    // for timing the rotation function
    LARGE_INTEGER perfreq;
    if(QueryPerformanceFrequency(&perfreq))
    {
        gdTicksPerSec = (double)perfreq.LowPart;
        gbTimeFunction = true;
    }
    else
    {
        MessageBox(NULL, 
            "High resolution timer not available",
            "Warning", MB_OK);
    }
         
    // Create our source and destination DIB`s
    gDibSrc = new CDIB();
    gDibDst = new CDIB();
 
    if(gDibSrc && gDibDst)
    {
        // Create Window
        hwnd = CreateWindow(
           __FILE__, "Fast Bitmap Rotation", WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 
           NULL, NULL, hInstance, NULL);
 
        ShowWindow (hwnd, iCmdShow) ;
        UpdateWindow (hwnd) ;
 
        while (GetMessage (&msg, NULL, 0, 0))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }       
 
        // cleanup DIB`s
        delete gDibSrc;
        delete gDibDst;
    }
    return msg.wParam;
}
/////////////////////////////////////////////////////////////////
double GetTimer()
{
    if(gbTimeFunction)
    {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
 
        return (double)time.QuadPart / gdTicksPerSec;
    }
    return 0.0;
}
/////////////////////////////////////////////////////////////////
void Update(HDC hdc)
{
    // ZeroMemory(gDibDst->m_pSrcBits, gDibDst->m_iHeight * gDibDst->m_iSWidth); // clear

    double dStartT = GetTimer();
 
    // Call RotateWrapFill routine, using center of the window and the
    // center of the source image as the points to rotate around
    // RotateDrawWithClip(
    RotateWrapFill(
        gDibDst->m_pSrcBits, gDibDst->m_iWidth, 
        gDibDst->m_iHeight, gDibDst->m_iSWidth,
        gDibSrc->m_pSrcBits, gDibSrc->m_iWidth, 
        gDibSrc->m_iHeight, gDibSrc->m_iSWidth,
        (float)gDibDst->m_iWidth/2, (float)gDibDst->m_iHeight/2,
        (float)gDibSrc->m_iWidth/2, (float)gDibSrc->m_iHeight/2,
        gdAngle, gdScale);
     
    // Change direction of the scale
    if(gdScale <= _MINSCALE || gdScale >= _MAXSCALE)
    {
        gdScaleDir *= -1.0;
    }
 
    // Update angle and scale
    gdScale += gdScaleDir;
    gdAngle += 0.02f;
     
    double dUpdateT = GetTimer();
 
    // Copy our rotated image to the screen
    BitBlt(hdc, 0, 0, gDibDst->m_iWidth, gDibDst->m_iHeight, 
            gDibDst->m_hdc, 0, 0, SRCCOPY);
     
    double dRenderT = GetTimer();
 
    // Print function timing satistics
    if(gbTimeFunction)
    {
        char szBuffer[256];
        TextOut(hdc, 5, 5, szBuffer, 
             sprintf(szBuffer, "Update took %3.6f (~%3.2ffps)",
             dUpdateT-dStartT, 1.0 / (dUpdateT-dStartT)));
        TextOut(hdc, 5, 20, szBuffer, 
             sprintf(szBuffer, "Render took %3.6f (~%3.2ffps)",
             dRenderT-dUpdateT, 1.0 / (dRenderT-dUpdateT)));
    }
}
////////////////////////////////////////////////////////////////
BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{   
    BOOL bSuccess = FALSE;
 
    // Load test bitmap
    HBITMAP hbm = (HBITMAP)LoadImage(NULL, SZIMAGE, IMAGE_BITMAP,
                                     0, 0, LR_LOADFROMFILE);
 
    if(hbm)
    {
        // Map the bitmap into a dc
        HDC hdc = CreateCompatibleDC(NULL);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);
         
        // Get info about this bitmap       
        BITMAP bm;      
        if(GetObject(hbm, sizeof(BITMAP), &bm) != 0)
        {
            // Convert the bitmap into DIB of known colour depth
            if(gDibSrc->Create(hdc, 0, 0, 
                bm.bmWidth, bm.bmHeight))
            {       
                // Start the update timer
                SetTimer(hwnd, 0, 100, NULL);
                bSuccess = TRUE;
            }
             
            // cleanup hdc
            SelectObject(hdc, hbmOld);
            DeleteDC(hdc);
        }
        // delete the loaded image
        DeleteObject(hbm);
    }   
    else
    {
        char szError[512];
        wsprintf(szError, "Error loading image %s", SZIMAGE);
        MessageBox(hwnd, szError, "oops", MB_OK);
    }
    return bSuccess;
}
/////////////////////////////////////////////////////////////////
void OnSize(HWND hwnd, UINT state, int x, int y)
{
    // Recreate the window DIB to match the size of the window
    gDibDst->Create(NULL, 0, 0, x, y);  
}
/////////////////////////////////////////////////////////////////
BOOL OnEraseBkGnd(HWND hwnd, HDC hdc)
{   
    // clearing the bk results in tears.
    return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnPaint(HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;
 
    hdc = BeginPaint (hwnd, &ps);
    Update(hdc);
    EndPaint (hwnd, &ps) ;
}
 
/////////////////////////////////////////////////////////////////
BOOL OnTimer(HWND hwnd, UINT id)
{   
    HDC hdc = GetDC(hwnd);
    Update(hdc);
    ReleaseDC(hwnd, hdc);
    return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}
/////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, 
                            WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        HANDLE_MSG( hwnd, WM_CREATE,     OnCreate );
        HANDLE_MSG( hwnd, WM_SIZE,       OnSize );
        HANDLE_MSG( hwnd, WM_PAINT,      OnPaint );
        HANDLE_MSG( hwnd, WM_ERASEBKGND, OnEraseBkGnd );
        HANDLE_MSG( hwnd, WM_TIMER,      OnTimer );
        HANDLE_MSG( hwnd, WM_DESTROY,    OnDestroy );
    }
 
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}
/////////////////////////////////////////////////////////////////
//End of File
#endif  //_WIN32

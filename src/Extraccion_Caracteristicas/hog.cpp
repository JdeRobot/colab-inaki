/*
*
* Copyright 2014-2016 Ignacio San Roman Lana
*
* This file is part of OpenCV_ML_Tool
*
* OpenCV_ML_Tool is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* OpenCV_ML_Tool is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OpenCV_ML_Tool. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* isanromanlana@gmail.com
*/

#include "hog.h"

MLT::HOG::HOG(Size winSize, Size block_stride, double win_sigma, double threshold_L2hys, bool gamma_correction, int nlevels)
{
    //ocl::HOGDescriptor::HOGDescriptor(Size win_size = Size(64, 128), Size block_size = Size(16, 16),
    //                                  Size block_stride = Size(8, 8), Size cell_size = Size(8, 8), int nbins = 9, double win_sigma = DEFAULT_WIN_SIGMA,
    //                                  double threshold_L2hys = 0.2, bool gamma_correction = true, int nlevels = DEFAULT_NLEVELS)
    //Parameters:
    //win_size – Detection window size. Align to block size and block stride.
    //block_size – Block size in pixels. Align to cell size. Only (16,16) is supported for now.
    //block_stride – Block stride. It must be a multiple of cell size.
    //cell_size – Cell size. Only (8, 8) is supported for now.
    //nbins – Number of bins. Only 9 bins per cell are supported for now.
    //win_sigma – Gaussian smoothing window parameter.
    //threshold_L2hys – L2-Hys normalization method shrinkage.
    //gamma_correction – Flag to specify whether the gamma correction preprocessing is required or not.
    //nlevels – Maximum number of detection window increases.

    hog.winSize.width = winSize.width;
    hog.winSize.height = winSize.height;
    hog.blockStride.width = block_stride.width;
    hog.blockStride.height = block_stride.height;
    hog.winSigma = win_sigma;
    hog.L2HysThreshold = threshold_L2hys;
    hog.gammaCorrection = gamma_correction;
    hog.nlevels = nlevels;
}

MLT::HOG::~HOG()
{

}

int MLT::HOG::Extract(vector<Mat> imagenes, vector<Mat> &descriptores)
{
    //void ocl::HOGDescriptor::getDescriptors(const oclMat& img, Size win_stride, oclMat& descriptors, int descr_format = DESCR_FORMAT_COL_BY_COL)
    //Parameters:
    //img – Source image. See ocl::HOGDescriptor::detect() for type limitations.
    //win_stride – Window stride. It must be a multiple of block stride.
    //descriptors – 2D array of descriptors.
    //descr_format –
    //Descriptor storage format:

    //DESCR_FORMAT_ROW_BY_ROW - Row-major order.
    //DESCR_FORMAT_COL_BY_COL - Column-major order.


    if (imagenes.size() == 0)
    {
        cout << "ERROR en Extract: imagenes esta vacio" << endl;
        this->error=1;
        return this->error;
    }

    if (hog.winSize.height > imagenes[0].rows || hog.winSize.width > imagenes[0].cols)
    {
        cout << "ERROR en Extract: El tamaño de win_Size es mayor que el de las imagenes" << endl;
        this->error=1;
        return this->error;
    }

    if (hog.winSize.height < 0 || hog.winSize.width < 0)
    {
        cout << "ERROR en Extract: El tamaño de win_Size es negativo" << endl;
        this->error=1;
        return this->error;
    }

    descriptores.clear();

    for (uint i = 0; i < imagenes.size(); i++)
    {
        Mat imagen;
        Mat img_gray;

        imagenes[i].copyTo(imagen);

        if (imagen.channels() == 3)
            cvtColor(imagen, img_gray, COLOR_BGR2GRAY);
        else
            imagen.copyTo(img_gray);

        img_gray.convertTo(img_gray, CV_8UC1);
        vector<float> descriptorsValues;
        hog.compute(img_gray, descriptorsValues);

        Mat desc(descriptorsValues);
        Mat trans;
        transpose(desc,trans);
        descriptores.push_back(trans);
    }

    if (descriptores.empty())
    {
        cout << "ERROR en Extract: descriptores vacio" << endl;
        this->error=1;
        return this->error;
    }
    valores = descriptores;
    this->error=0;
    return this->error;
}

int MLT::HOG::Mostrar(vector<Mat> images, int scaleimage, int scalelines)
{
    Mat origImg;
    vector<float> descriptorValues;
    for (uint i = 0; i < images.size(); i++)
    {
        descriptorValues.clear();

        Mat visual_image;
        images[i].copyTo(origImg);
        resize(origImg, visual_image, Size(origImg.cols * scaleimage, origImg.rows * scaleimage));

        int gradientBinSize = 9;
        float radRangeForOneBin = 3.14/(float)gradientBinSize;
        int cells_in_x_dir = origImg.cols / hog.cellSize.width;
        int cells_in_y_dir = origImg.rows / hog.cellSize.height;

        float*** gradientStrengths = new float**[cells_in_y_dir];
        int** cellUpdateCounter = new int*[cells_in_y_dir];
        int cont = 0;

        for(int j = 0; j < valores[i].cols; j++)
            descriptorValues.push_back(valores[i].at<float>(0,j));

        for (int y = 0; y < cells_in_y_dir; y++)
        {
            gradientStrengths[y] = new float*[cells_in_x_dir];
            cellUpdateCounter[y] = new int[cells_in_x_dir];
            for (int x = 0; x < cells_in_x_dir; x++)
            {
                gradientStrengths[y][x] = new float[gradientBinSize];
                cellUpdateCounter[y][x] = 0;

                for (int bin = 0; bin < gradientBinSize; bin++)
                {
                    gradientStrengths[y][x][bin] = descriptorValues[cont];
                    cont++;
                }
            }
        }

        for (int celly = 0; celly < cells_in_y_dir; celly++)
        {
            for (int cellx = 0; cellx < cells_in_x_dir; cellx++)
            {
                int drawX = cellx * hog.cellSize.width;
                int drawY = celly * hog.cellSize.height;

                int mx = drawX + hog.cellSize.width / 2;
                int my = drawY + hog.cellSize.height / 2;

                rectangle(visual_image,
                          Point(drawX * scaleimage, drawY * scaleimage),
                          Point((drawX + hog.cellSize.width) * scaleimage,
                                (drawY + hog.cellSize.height) * scaleimage),
                          CV_RGB(100,100,100),
                          1);

                for (int bin = 0; bin < gradientBinSize; bin++)
                {
                    float currentGradStrength = gradientStrengths[celly][cellx][bin];
                    if (currentGradStrength == 0)
                        continue;

                    float currRad = bin * radRangeForOneBin + radRangeForOneBin/2;
                    float dirVecX = cos( currRad );
                    float dirVecY = sin( currRad );
                    float maxVecLen = hog.cellSize.width/2;
                    float scale = scalelines;
                    float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
                    float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
                    float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
                    float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;
                    line(visual_image,
                         Point(x1 * scaleimage,y1 * scaleimage),
                         Point(x2 * scaleimage,y2 * scaleimage),
                         CV_RGB(0, 0, 255),
                         1);
                }
            }
        }
        for (int y = 0; y < cells_in_y_dir; y++)
        {
            for (int x = 0; x < cells_in_x_dir; x++)
            {
                delete[] gradientStrengths[y][x];
            }
            delete[] gradientStrengths[y];
            delete[] cellUpdateCounter[y];
        }

        delete[] gradientStrengths;
        delete[] cellUpdateCounter;

        imshow("HOG", visual_image);

        waitKey(0);
    }
    this->error=0;
    return this->error;
}



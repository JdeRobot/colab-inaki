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

#ifndef DESCRIPTORES_H
#define DESCRIPTORES_H

#include <opencv2/xfeatures2d.hpp>
#include "descriptor.h"

using namespace std;
using namespace cv;

namespace MLT
{
    class Puntos_Caracteristicos: public Descriptor
    {
    public:
        Puntos_Caracteristicos(String detectorType = "SURF", String extractorType = "SURF", float paramDetector = 1000);

        int Extract(vector<cv::Mat> images, vector<cv::Mat>& descriptores) override;

        void Mostrar(vector<Mat> images);
    private:
        std::vector<vector<cv::KeyPoint>> keypointsI;
        cv::String detectorType;
        cv::String extractorType;
        float paramDetector;
    };
}

#endif // DESCRIPTORES_H

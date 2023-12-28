__kernel void mandelbrot(__global uchar4* pixels, int width, int height, int maxIterations, float realMin, float realMax, float complexMin, float complexMax)
    {
        int globalX = get_global_id(0);
        int globalY = get_global_id(1);

        float xComplex = globalX * ((realMax - realMin) / (width - 1)) + realMin;
        float yComplex = globalY * (complexMax - complexMin) / (height - 1) + complexMin;
        float2 complexNumber = (float2)(xComplex, yComplex);
        float2 iterationComplex = complexNumber;
        int iterations = 0;

        while (length(iterationComplex) < 2.0 && iterations < maxIterations)
        {
            iterations++;
            iterationComplex = iterationComplex * iterationComplex + complexNumber;
        }

        uchar4 color;
        color.x = (uchar)(iterations % 256);
        color.y = (uchar)((iterations / 256) % 256);
        color.z = (uchar)((iterations / (256 * 256)) % 256);
        color.w = 255;

        // Write color to pixel buffer
        int index = globalY * width + globalX;
        pixels[index] = color;
    }
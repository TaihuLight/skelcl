int func(__local float* img)
{
    return (getElem2D(img,0,10));
    //+getElem2DGlobal(img,0,1,cols)+getElem2DGlobal(img,-1,0,cols)+getElem2DGlobal(img,0,-1,cols))/4;
}

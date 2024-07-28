package io.bluerose73.fiji.dcimg;

import java.nio.file.Path;
import java.nio.file.Paths;

public class DcimgReader {
    static {
        Path fijiPath = Paths.get(System.getProperty("fiji.dir"));
        Path dcimgJniPath = Paths.get(fijiPath.toString(), "plugins", "DCIMG", "dcimg_jni.dll");
        Path dcimgApiPath = Paths.get(fijiPath.toString(), "plugins", "DCIMG", "dcimgapi.dll");
        System.load(dcimgApiPath.toString());
        System.load(dcimgJniPath.toString());
    }

    public static class Size {
        public int width;
        public int height;
        public int nFrames;
        public int bitsPerPixel;
    }

    public static native Size ReadSize(String filepath);

    public static native byte[] ReadFrame(String filepath, int FrameId);
}

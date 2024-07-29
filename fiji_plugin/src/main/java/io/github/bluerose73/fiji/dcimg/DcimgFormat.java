package io.github.bluerose73.fiji.dcimg;

import org.scijava.plugin.Plugin;

import io.scif.Format;
import io.scif.ImageMetadata;
import io.scif.AbstractFormat;
import io.scif.AbstractMetadata;
import io.scif.AbstractParser;
import io.scif.ByteArrayPlane;
import io.scif.ByteArrayReader;
import io.scif.util.FormatTools;
import io.scif.FormatException;
import io.scif.config.SCIFIOConfig;
import org.scijava.io.handle.DataHandle;
import org.scijava.io.location.Location;

import net.imagej.axis.Axes;
import net.imglib2.Interval;

import java.io.IOException;

@Plugin(type = Format.class, name = "DCIMG")
public class DcimgFormat extends AbstractFormat {

    @Override
    public String getFormatName() {
        return "DCIMG";
    }

    @Override
    protected String[] makeSuffixArray() {
        log().info("DCIMG suffix called!");
        return new String[] { "dcimg" };
    }

    public static class Metadata extends AbstractMetadata {
        @Override
        public void populateImageMetadata() {
            log().info("Populating Metadata");
            final ImageMetadata iMeta = get(0);
            int bpp = iMeta.getBitsPerPixel();
            iMeta.setAxisTypes(Axes.X, Axes.Y, Axes.Z);
            iMeta.setPlanarAxisCount(2);

            switch (bpp) {
                case 8:
                    iMeta.setPixelType(FormatTools.UINT8);
                    break;
                case 16:
                    iMeta.setPixelType(FormatTools.UINT16);
                    break;
                default:
                    log().warn("Unknown bits per pixel: " + String.valueOf(bpp));
                    iMeta.setPixelType(FormatTools.UINT8);
                    break;
            }

            iMeta.setLittleEndian(true);
            iMeta.setMetadataComplete(true);
            iMeta.setIndexed(false);
        }
    }

    public static class Parser extends AbstractParser<Metadata> {
        @Override
        public void typedParse(final DataHandle<Location> dataHandle, final Metadata meta,
                final SCIFIOConfig config) throws IOException, FormatException {
            meta.createImageMetadata(1);
            final ImageMetadata iMeta = meta.get(0);

            String path = dataHandle.get().getURI().getPath();
            path = path.replaceAll("^/+", "");
            log().info("dcimg path is " + path);

            DcimgReader.Size imgSize = DcimgReader.ReadSize(path);

            if (imgSize.height == 0) {
                log().warn("DCIMG get size failed. Does the file exist?");
                return;
            }

            iMeta.addAxis(Axes.X, imgSize.width);
            iMeta.addAxis(Axes.Y, imgSize.height);
            iMeta.addAxis(Axes.Z, imgSize.nFrames);
            iMeta.setBitsPerPixel(imgSize.bitsPerPixel);
        }
    }

    public static class Reader extends ByteArrayReader<Metadata> {
        public ByteArrayPlane openPlane(int imageIndex, long planeIndex,
                ByteArrayPlane plane, Interval bounds, SCIFIOConfig config)
                throws FormatException, IOException {
            final Metadata meta = getMetadata();
            String path = meta.getSource().get().getURI().getPath();
            path = path.replaceAll("^/+", "");
            final byte[] frameData = DcimgReader.ReadFrame(path, (int) planeIndex);
            plane.setData(frameData);
            return plane;
        }

        @Override
        protected String[] createDomainArray() {
            return new String[0];
        }
    }
}

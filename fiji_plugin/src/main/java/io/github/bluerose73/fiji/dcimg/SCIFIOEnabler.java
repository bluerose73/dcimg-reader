package io.github.bluerose73.fiji.dcimg;

import org.scijava.event.EventHandler;
import org.scijava.plugin.Plugin;
import org.scijava.service.Service;
import org.scijava.service.event.ServicesLoadedEvent;
import org.scijava.service.AbstractService;

import net.imagej.ImageJService;
import net.imagej.legacy.ImageJ2Options;

@Plugin(type = Service.class)
public class SCIFIOEnabler extends AbstractService implements ImageJService {

    @EventHandler
    protected void onEvent(ServicesLoadedEvent event) {
        ImageJ2Options imageJ2Options = new ImageJ2Options();
        imageJ2Options.setContext(this.context());
        imageJ2Options.setInput("sciJavaIO", true);
        imageJ2Options.run();
        log().info("SCIFIO Enabler run!");
    }

    @Override
    public void initialize() {
        log().info("SCIFIO Enabler init");
    }
}

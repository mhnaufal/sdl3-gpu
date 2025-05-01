package org.libsdl.bubuk;

import org.libsdl.app.SDLActivity;

public class HelloWorldActivity extends SDLActivity {
    protected String[] getLibraries() {
        return new String[] { "SDL3", "bubuk" };
    }
}

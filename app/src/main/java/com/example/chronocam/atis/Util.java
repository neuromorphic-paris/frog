package com.example.chronocam.atis;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static android.content.ContentValues.TAG;

class Util {

    static String copyResource(Context context, String resource) {
        if (resource.equals(""))
            return "";
        AssetManager assetManager = context.getAssets();
        try {
            String resourceLocation = context.getFilesDir().getPath() + "/" + resource;
            copyAsset(assetManager, resource, resourceLocation);
            return resourceLocation;
        } catch (Exception e) {
            Log.d("Util", "CopyResource() EXCEPTION");
            e.printStackTrace();
        }
        return "";
    }

    private static boolean copyAsset(AssetManager assetManager, String fromAssetPath, String toPath) throws IOException {
        InputStream in = null;
        OutputStream out = null;
        in = assetManager.open(fromAssetPath);
        new File(toPath).createNewFile();
        out = new FileOutputStream(toPath);
        copyFile(in, out);
        in.close();
        in = null;
        out.flush();
        out.close();
        out = null;
        Log.d(TAG, "copyAsset() " + fromAssetPath + " --> " + toPath);
        return true;
    }

    private static void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

}

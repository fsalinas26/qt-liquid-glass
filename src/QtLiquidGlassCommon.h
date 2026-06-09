#ifndef QTLIQUIDGLASS_COMMON_H
#define QTLIQUIDGLASS_COMMON_H

#if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_OSX
#endif

#ifdef PLATFORM_OSX
    // DO NOT include AppKit/Foundation here because this header 
    // is included by QtLiquidGlass.cpp which is a standard C++ file.
    
    // Forward declarations for internal functions used by the backend
    extern "C" {
        // nativeViewPtr is a void* (cast from NSView*)
        int AddGlassEffectView(void* nativeViewPtr,
                               bool opaque,
                               int titlebarStyle,
                               int dragBehavior);
        
        // Configure with raw color values (0.0 - 1.0) to avoid string parsing in ObjC
        void ConfigureGlassView(int viewId, double cornerRadius, bool hasTint, double r, double g, double b, double a);
        
        // Explicit setters for private and public properties
        void SetGlassViewVariant(int viewId, int variant);
        void SetGlassViewMaterial(int viewId, int material); // Fallback standard API
        void SetGlassViewBlendingMode(int viewId, int mode); // 0=BehindWindow, 1=WithinWindow
        void SetGlassViewScrim(int viewId, int scrim);
        void SetGlassViewSubdued(int viewId, int subdued);
        void SetGlassViewContentLensing(int viewId, int lensing);
        void SetGlassViewAdaptiveAppearance(int viewId, int appearance); // 0=light,1=dark,2=auto
        void SetGlassViewInteractionState(int viewId, int state);       // 0=normal,1=hovered

        void RemoveGlassEffectView(int viewId);
    }
#endif

#endif // QTLIQUIDGLASS_COMMON_H

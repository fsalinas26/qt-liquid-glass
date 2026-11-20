# QtLiquidGlass 0.4.1

QtLiquidGlass 0.4.1 is verified on macOS 27 and fixes two window-handling problems found during that audit. There are no public API changes; existing v0.4.0 source usage remains compatible.

## Fixed

- Leaving fullscreen no longer drops the transparent full-size titlebar. Qt rebuilds the window's style mask around fullscreen; the library now re-applies the requested titlebar and drag policy on exit and keeps the window at its previous size.
- Adding glass to a window whose native handle already exists and is opaque no longer leaves the window undraggable. The library keeps the application's titlebar in that case and logs a warning that names the cause and the fix.
- GlassDemo's Copy Config now emits `Material::ClearGlass`; the previous output did not compile.

## Changed

- The README records macOS 27.0.1 as the last verified runtime, notes the system Liquid Glass tint setting, and describes frameless windows as they are actually handled: the same sibling injection as standard windows, with the native wrapper only as a fallback.
- The README no longer describes `Material::Frosted` and `Material::Chromatic` as frosted. The variants they map to are clear, with a refracting rim, on both macOS 26.0 and 27.0.1; `Chromatic` adds chromatic aberration at the rim. The enum names and mappings are unchanged.
- The custom-shape regression test no longer fails when the private path getter returns nothing; the geometry checks are skipped in that case.

## Compatibility notes

- Verified on macOS 27.0.1 (26A434), built with the macOS 26 SDK. macOS 26 was not re-tested for this release.
- Every private selector the library uses is still present on macOS 27.0.1 with an unchanged signature.
- On macOS 27, glass follows the system-wide Liquid Glass tint setting. At higher tint levels materials render more opaque and look more alike. The library does not change this setting.
- `Material::ClearGlass` draws no glass on macOS 27.0.1: the private variant it maps to builds no material there, so the surface is fully transparent. The other 14 materials render. The mapping is unchanged in this release.
- `TitlebarStyle::Preserve` and `WindowDragBehavior::Preserve` still leave application-managed window state alone, including after fullscreen.
- Private AppKit material variants and experimental shape behavior can change between macOS releases.

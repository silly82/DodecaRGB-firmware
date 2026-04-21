# Konzept: Pseudo-DOOM auf DodecaRGB (Wahrsagekugel-Optik)

## Ziel

Ein humorvoller "DOOM"-Eindruck mit sehr geringer Komplexitaet. Kein echter Port, sondern ein stilisierter Renderer mit Kugelprojektion auf den Dodekaeder.

## Grundidee

- Eine niedrige virtuelle Ansicht (z. B. 64x32) simuliert einen einfachen Korridor/Room-Look.
- Ein sehr kleiner Pseudo-Raymarch/Pseudo-Raycast erzeugt Wandstreifen, Gegner-Silhouetten und Muzzle-Flash.
- Das Bild wird nicht faceweise, sondern ueber spharische Koordinaten auf alle LEDs gemappt.
- Dadurch wirkt es wie ein durchgehendes Bild in einer Glaskugel.

## Projektion auf den Dodekaeder

1. Fuer jede LED den 3D-Punkt `p = model().point(i)` lesen.
2. Normalisieren: `n = normalize(p)`.
3. In kugelfoermige UV-Koordinaten umrechnen:
   - `u = (atan2(n.y, n.x) + pi) / (2*pi)`
   - `v = acos(n.z) / pi`
4. Virtuelle Framebuffer-Farbe bei `(u, v)` sampeln und auf LED schreiben.

## Gameplay-Illusion (minimal)

- Zustandsautomat mit 3-4 States:
  - WALK
  - ENCOUNTER
  - SHOOT_FLASH
  - DAMAGE_FLASH
- Ein Button:
  - kurzer Druck = Schuss-Flash
  - langer Druck = Waffenfarbe wechseln (nur visuell)
- Keine echte Kollision, kein Pfadfinding.

## Performance-Ziel

- Soll auf Teensy 4.1 leicht laufen.
- Rechenaufwand linear in LED-Anzahl plus kleiner Renderer-Buffer.
- Kein Asset-Streaming, keine grossen Tabellen noetig.

## Minimaler Implementierungsplan

1. Neue Szene `src/scenes/doom_pseudo/doom_pseudo_scene.{h,cpp}`.
2. Parameter:
   - `speed`
   - `enemy_rate`
   - `brightness`
   - `fov`
3. Kleiner interner Framebuffer (z. B. 64x32, 8-bit oder CRGB).
4. Pro Tick:
   - Szene-Status updaten
   - Buffer rendern
   - Kugelprojektion auf LEDs anwenden

## Risiken

- Zu "busy" wirkendes Bild auf unregelmaessigem LED-Gitter.
- Bei zu viel Detail flimmert das Bild; deshalb sehr grobe Formen bevorzugen.

## Ergebnis

Sehr kleiner, wartbarer "DOOM-Gag" mit starkem Effekt, ohne echten Engine-Port.

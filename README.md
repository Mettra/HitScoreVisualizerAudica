# Audica HitScoreVisualizer
A mod to color the hit scores based on how high the score is!

## Installing
Make sure you have [Audica Mod Loader](https://github.com/Mettra/AudicaModLoader) installed.

Download the latest HitScoreVisualizer.dll from Releases, and put it in the Mods folder.


## Configuration
Upon launching Audica, a configuration file located under `Mods/Config/HitScoreVisualizer.json` should be generated, and looks like this:

```json
[
    {
        "color": "FF00FF",
        "cutoff": 0.4
    },
    {
        "color": "00FF4E",
        "cutoff": 0.7
    },
    {
        "color": "00FFDE",
        "cutoff": 0.9
    },
    {
        "color": "FFD800",
        "cutoff": 1.0
    }
]
```

The structure of this config is simple, each object in this list has a cutoff point and a color. The cutoff represents the highest possible score percent to have it be that color.

For example, the default config is: below 40% is red, between 40% and 70% is green, 70-90% is cyan, and 90%+ is gold.


## Building
```bash
git clone https://github.com/Mettra/HitScoreVisualizerAudica.git
cd HitScoreVisualizerAudica
git submodule update --init --recursive
./build.bat
```
#import "@preview/colorful-boxes:1.4.2": colorbox
#import "@preview/numbly:0.1.0": numbly
#import "@preview/touying:0.5.3": *
#import themes.metropolis: *
#import "@preview/callisto:0.2.5"

#show: metropolis-theme.with(
  aspect-ratio: "16-9",
  align: horizon,
  config-info(
    title: [Tof-prosjekt våren 2026 - Rapport],
    subtitle: [Støy og konsentrasjon],
    author: [Olai og Håvard],
    date: [2026-04-09],
    institution: [Amalie Skram VGS],
  ),
  //   config-colors(
  //     primary: rgb("#d3869b"),
  //     primary-light: rgb("#d3869b"),
  //     secondary: rgb("#d3869b"),
  //     neutral-lightest: rgb("#282828"),
  //     neutral-dark: rgb("#dbd1c7"),
  //     neutral-darkest: rgb("#ebdbb2"),
  //   ),
)

#set text(lang: "nb", font: "DejaVu Sans")

#set heading(numbering: numbly("{1}.", default: "1.1"))

#show raw: set text(font: "JetBrainsMono NF")
#show raw.where(block: true): x => {
  set text(size: 0.8em)
  block(
    fill: luma(245),
    stroke: 1pt + luma(200),
    inset: 10pt,
    radius: 4pt,
    width: 100%,
    x,
  )
}
#show raw.where(block: false): x => {
  h(1pt)
  box(
    fill: luma(240),
    stroke: 1pt + luma(200),
    inset: (x: 3pt),
    outset: (y: 3pt),
    radius: 2pt,
    x,
  )
  h(1pt)
}

#title-slide()

= Innhold <touying:hidden>

#outline(title: none, indent: 1em, depth: 2)

= Introduksjon

== Hypotese
Undersøke korrelasjonen mellom støy og konsentrasjon i klasserommet.

#colorbox(title: "Hypotese", color: "blue")[
  Økt støynivå i klasserommet fører til lavere selvrapportert konsentrasjon
]

Andre underhypoteser

- Høyere temperatur i klasserommet vil føre til høyere støynivå
- Lavere lysnivå vil medføre mindre støy og bedre konsentrasjon

= Metode

== Kravspesifikasjoner

Innsamling av empirisk data fra klasserom:

- Temperatur
- Støynivå
- Lysnivå

Lagres til en CSV-fil for behandling

== Komponenter

#slide(composer: (1fr, auto))[
  Komponenter per arduino:

  - Arduino MKR NB 1500
  - Sd-kort (formatert til FAT32)
  - MKR MEM Shield
  - TEMT6000 lyssensor
  - BME280 temperatursensor
  - IMNP441 mikrofon

  - 5V mobillader + kabel
  - Koblingsbrett
  - Diverse kabler
][
  #figure(
    image("Assets/arduino.png", width: 20em),
    caption: [Ferdig arduino],
  )
]

#slide[
  === Oppsett av arduinoer

  Pseudokode som viser omtrent hva som blir brukt:

  ```cpp
  void loop() {
    unsigned long now = millis();
    sampleImnp441();

    if (now - lastWrite >= writeInterval) {
      lastWrite = now;

      float temp = bme280.readTemperature();
      float humidity = bme280.readHumidity();
      float light = analogRead(TEMT6000_PIN);
      float sound = readImnp441();

      // Print the data to file
    }
  }
  ```
]

#slide(align: top)[
  == Testing av sensorer

  Bruker arduino med sensorene beskrevet over. Målte først over helgen.

  #figure(
    image("Assets/weekend-hist.png", width: 30em),
    caption: [Histogram over data fra helgen],
  )
]

#slide[
  Innlesing av data

  > Ignorerer de største, urealistiske avvikene.

  ```py
  d = "20260313T1512-weekend-baseline"

  dfs = []
  for i in range(2):
      df = pd.read_csv(f"{d}/a{i+1}-data.csv")
      df = df[15:-15].copy()
      df.loc[df["Humidity (%)"] > 90, "Humidity (%)"] = np.nan
      df.loc[df["Temperature (C)"] > 25, "Temperature (C)"] = np.nan
      df.loc[df["Temperature (C)"] < 15, "Temperature (C)"] = np.nan
      df = df.dropna()
      del df["Timestamp"]
      dfs.append(df)
  ```
]

#slide(align: top)[
  Lyden er i intervallet $[0.0, 50 thin 000]$. Arduinoene er start sett rundt samme verdi med litt variasjon

  #figure(
    image("Assets/weekend-hist2.png", width: 30em),
    caption: [Sammenligning mellom arduinoer over helgen],
  )

  BME280 er generelt litt lavere fra a2, resten er likt
]

#slide(align: center)[
  #grid(
    columns: (auto, auto),
    [
      #figure(
        image("Assets/weekend-temp.png", height: 13em),
        caption: [Temperatur over helgen],
      )
    ],
    [
      #figure(
        image("Assets/weekend-humidity.png", height: 13em),
        caption: [Fuktighet over helgen],
      )
    ],
  )

  Her er de relative forskjellene like. De første par verdiene er feil. Dette går til "oppvarming" av sensorene. Fjernes fra datasettet.
]



#slide(composer: (1fr, auto))[
  Velger intervallet $[20 thin 000, 50 thin 000]$.
  Filtrerer ut store avvik der $x < 3 thin 000 or x > 10 thin 000$

  ```py
    night = [
      df.iloc[20000:50000]
        .copy()
        .reset_index(drop=True)
        ["Sound"]
        .loc[lambda x: (x >= 3000) & (x <= 10000)]
        .rolling(200)
        .mean()
      for df in dfs
  ]
  ```
][
  #figure(
    image("Assets/sound-baseline-1.png", width: 15em),
    caption: [Lydnivå over helgen],
  )
]

#slide(composer: (1fr, auto))[
  ```py
  >>> abs(night[1].mean() - night[0].mean())
  np.float64(321.79450492549495)

  >>> night[0].corr(night[1])
  np.float64(0.004969202453485703)
  ```

  #colorbox(title: "Corr", color: "green")[
    #text(size: 0.9em)[
      Viser korrelasjonen mellom datasett - om $a$ øker når $b$ øker og motsatt.
    ]
  ]

  Det er tydelig korrelasjon mellom dataen, men de er forskjøvet i forhold til hverandre.
][
  #figure(
    image("Assets/sound-baseline-2.png", width: 15em),
    caption: [Lydnivå over helgen],
  )
]

#slide(composer: (1fr, auto))[
  Så tidligere at lys er mest likt mellom arduinoer. Forskyver derfor basert på det.

  ```py
  from scipy import signal

  parameter = "Light"
  sig1 = dfs[0][parameter] - dfs[0][parameter].mean()
  sig2 = dfs[1][parameter] - dfs[1][parameter].mean()

  corr = signal.correlate(sig1, sig2, mode='full')
  lags = signal.correlation_lags(len(sig1), len(sig2), mode='full')
  best_lag = lags[np.argmax(corr)]

  corrected = [dfs[0].copy(), dfs[1].shift(periods=best_lag)]
  ```


  ```py
  >>> night[0].corr(night[1])
  np.float64(0.9857182550092554)
  ```
][
  #figure(
    image("Assets/sound-baseline-3.png", width: 15em),
    caption: [Lydnivå over helgen],
  )
]

#slide(align: top)[
  === Konklusjon

  Sensorene måler litt ulike konstante verdier, men har samme relative forskjell etter forskyvningen. Tar derfor gjennomsnittet av disse. For oss er kun forskjellene relevant og ikke de nøyaktige verdiene.

  Dette kan da brukes videre med de reelle datasettene.
]

== Spørreundersøkelse

Inneholder følgende spørsmål:

- Hvor konsentrert var du i timen?
- Hvor mye arbeid gjorde du?

Undersøkelsen besvares etter hver målte skoletime.

== Analyse

Reduserer antall rader ved å gruppere i blokker på 400 rader.

```
for d in datasets:
    for each arduino:
        read data
        remove extremes
        align datasets by light
        drop first and last 15 rows
        drop invalid values

    get mean of both dataframes
    reduce number of samples
    add survey

concatenate datasets
```

= Resultater

#slide(align: center + horizon)[
  Samlet inn fra 12 individuelle timer, med om lag 100 svar totalt på undersøkelsen, fokusert på tre fag:

  #table(
    columns: 2,
    align: center,
    inset: 10pt,
    [*Fag*], [*Antall Målinger*],
    [IT1], [3],
    [ToF], [4],
    [Tysk], [5],
  )

  I tillegg til samtlige ubrukte testmålinger, blant annet over natten og uten tilknyttet spørreundersøkelse. Totalt 215 timer med målinger.
]

== Korrelasjoner

#slide(composer: (auto, 1fr), align: top)[
  #alternatives[
    #callisto.display(
      "tysk-corr",
      nb: json("data/analysis.ipynb"),
    )
  ][
    #callisto.display(
      "tysk-corr2",
      nb: json("data/analysis.ipynb"),
    )
  ]
][
  === Tysk

  ```py
  corr = df[df["Subject"] == "tysk"].corr(numeric_only=True)
  ax = sns.heatmap(corr)
  ```

  #pause

  Mer støy $arrow$ bedre konsentrasjon, men mindre arbeid.

  Kanskje "arbeid" er en bedre variabel?

  Temperatur har _sterk_ korrelasjon. Samme gjelder lys.
]

#slide(composer: (1fr, 1fr, 1fr), align: left)[
  *Tysk*
  #callisto.display(
    "tysk-corr",
    nb: json("data/analysis.ipynb"),
  )
  - Muntlig fag
][
  *IT1*
  #callisto.display(
    "it1-corr",
    nb: json("data/analysis.ipynb"),
  )
  - Teoretisk fag
  - Individuelt arbeid
][
  *ToF*
  #callisto.display(
    "tof-corr",
    nb: json("data/analysis.ipynb"),
  )
  - Praktisk fag
]

== Refleksjon

=== Forbedringer

- Kontrollgruppe?
- Redusere feilkilder
- Spørreundersøkelsen
  - Motivasjon?
  - Kalibrering og kontroll

== Konklusjon

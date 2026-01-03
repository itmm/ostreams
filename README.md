# Eigene `std::ostream`-Klassen

Die `iostream`-Bibliothek ist ein fester Bestandteil von C++. Die erste
C++-Installation, von der ich zumindest Tests gelesen habe [1], war
Lattice C++ 1.0 für den Commodore Amiga. Bereits in dieser Distribution
waren `iosteam`s enthalten (so zumindest der Test).

Bei all ihrer Flexibilität und Erweiterbarkeit, gibt es doch nur wenige,
die eigene Subtypen von `std::istream` und `std::ostream` erstellen. Dabei
ist der Aufwand gar nicht so groß. Damit können wie etwa in Java eigene
Implementationen mit nur wenig Zeilen erzeugt werden.

Dieser Artikel beschäftigt sich mit `std::ostream`, da dieser etwas leichter
erweitert werden kann.


## Etwas Hintergrund

Der `std::ostream` kann für formatierte und unformatierte Ausgabe verwendet
werden. Er kommt in vielen C++ Programmen vor, um als `std::cout` eine
Abstraktion für die Standard-Ausgabe zu bilden. Ebenso begegnen wir Subtypen
von `std::ostream`, besonders der Abstraktion `std::ofstream` für Dateien
oder `std::ostringstream`, um Ausgaben in Strings umzuleiten. Letzteres hilft
uns besonders beim Erstellen von Unit Tests.

Es spricht jedoch nichts dagegen, um auch eigene Subtypen zu bilden. Diese
schmiegen sich dann harmonischer in das Bibliotheksgefüge ein und können
leicht wiederverwendet werden.

Grundsätzlich benötigt ein `std::ostream` um zu funktionieren noch einen
`std::streambuf`. Der `std::ostream` kümmert sich um das Formatieren und
Aufbereiten der Daten, der `std::streambuf` legt die daraus resultierenden
Zeichenketten ab. Hauptsächlich wird unsere Arbeit im `std::streambuf`
stattfinden.


## Dateien mit Befehlen

Als praktisches Beispiel habe ich mit folgende Problemstellung heraus gesucht:
Es soll möglich sein, in der Ausgabe zusätzliche Befehle einzubauen. Diese
bestehen immer aus einer Zeile und haben die Form

`%name argumente...\n`

Damit das funktioniert, müssen wir Prozentzeichen, die am Anfang einer Zeile
in der Ausgabe stehen, maskieren. Sonst werden sie beim Lesen als Befehle
interpretiert. Um möglichst wenig Sonderzeichen zu verbrauchen, doppeln wir
einfach das Zeichen. Aus

```
abc
%def
```

wird dann

```
abc
%%def
```

und falls bereits mehrfache Prozentzeichen am Zeilenanfang vorkommen, wird
ein weiteres dazu gesetzt.

Das ist die Aufgabe unseres `std::ostream`s: leite alle Zeichen an einen
anderen `std::ostream` weiter, aber verdoppele dabei Prozentzeichen, wenn
sie am Anfang einer Zeile stehen.

Hier ist ein entsprechender Unit Test in der Datei `t_ostream.cpp`:

```c++
#include <cassert>
#include <sstream>

#include "ostream.h"

static void test_ostream(const std::string& in, const std::string& expected) {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << in;
	assert(out.str() == expected);
}

int main() {
	test_ostream("abc\ndef\n", "abc\ndef\n");
	test_ostream("abc\n%def\n", "abc\n%%def\n");
	test_ostream("abc\n%%def\n", "abc\n%%%def\n");
	test_ostream("a%c", "a%c");
}
```

## Die Umsetzung

In der Header-Datei deklariere ich die neue Klasse `marked_files::ostream`,
welche nach außen hin ein `std::ostream` ist. Sie ist jedoch gleichzeitig
ein `std::streambuf`, da wir dessen Methode `overflow` überschreiben müssen.

Den Trick mit der privaten Basis-Klasse habe ich aus einem Stack Overflow
Post gelernt [2]. Hier ist die Header-Datei `ostream.h`

```c++
#pragma once

#include <iostream>

namespace marked_files {
	class ostream: private std::streambuf, public std::ostream {
			std::ostream& forward_;
			int_type last_ = '\n';

			int_type overflow(int_type ch) override;

		public:
			ostream(std::ostream& forward):
				std::ostream { this }, forward_ { forward }
			{ }
	};
};
```

Die private Basisklasse `std::streambuf` muss vor `std::ostream` stehen.
Denn sie muss vor `std::ostream` initiasiliert werden. `std::ostream` erhält
den initialisierten `std::streambuf` als Argument im Konstruktor

Die Implementierung in `ostream.cpp` sieht so aus:

```c++
#include "ostream.h"

namespace marked_files {
	ostream::int_type ostream::overflow(int_type ch) {
		if (ch == '%' && last_ == '\n') { 
			if (! forward_.put(ch)) { return traits_type::eof(); }
		}
		if (! forward_.put(ch)) { return traits_type::eof(); }
		last_ = ch;
		return 0;
	}
}
```

[1] Lattice C++ für den Amiga, Ralph Babel, Amiga Magazin 4/1989,
    Markt & Technik Verlag, München

[2] How to inherit from std::ostream?, Answer form Ben and aSemy,
	 Stack Overflow, https://stackoverflow.com/questions/772355

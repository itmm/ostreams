# Eigene `std::ostream`-Klassen

Die `iostream`-Bibliothek ist ein fester Bestandteil von C++. Die erste
C++-Installation, von der ich zumindest Tests gelesen habe [1], war
Lattice C++ 1.0 für den Commodore Amiga. Bereits diese Distribution
enthielt die `iosteam`-Klassen (so zumindest der Test).

Bei all ihrer Flexibilität und Erweiterbarkeit, gibt es doch nur wenige,
die eigene Sub-Typen von `std::basic_istream` und `std::basic_ostream`
erstellen. Dabei ist der Aufwand gar nicht so groß. Damit können wir wie etwa
in Java eigene Implementationen mit nur wenig Zeilen erzeugen.

In diesem Artikel implementiere ich einen eigenen Sub-Typ von
`std::basic_ostream`, da ich diese Klasse etwas leichter erweitern kann. Der
Sub-Typ von `std::basic_istream` folgt in einem späteren Beitrag.


## Etwas Hintergrund

Der `std::basic_ostream` orchestriert formatierte und unformatierte Ausgaben.
Er kommt in vielen C++ Programmen vor, um als `std::cout` eine Abstraktion für
die Standard-Ausgabe zu bilden. Weitere Sub-Typen von `std::basic_ostream`
sind `std::ofstream` für Dateien und `std::ostringstream`, um Ausgaben in
Zeichenketten umzuleiten. Letzteres hilft uns besonders beim Erstellen von
Unit Tests.

Es spricht jedoch nichts dagegen, um auch eigene Sub-Typen zu bilden. Diese
schmiegen sich dann harmonisch in das Gefüge der Standard-Bibliotheken ein und
können leicht wiederverwendet werden.

Grundsätzlich benötigt ein `std::basic_ostream` noch einen
`std::basic_streambuf` um zu funktionieren. Der `std::basic_ostream` kümmert
sich um das Formatieren und Aufbereiten der Daten, der `std::basic_streambuf`
verwaltet die daraus resultierenden Zeichenketten. Hauptsächlich findet unsere
Arbeit im `std::basic_streambuf` statt.


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

Und falls bereits mehrfache Prozentzeichen am Zeilenanfang vorkommen, wird
ein weiteres dazu gesetzt.

Das ist die Aufgabe unseres `basic_ostream`s: leite alle Zeichen an einen
anderen `std::basic_ostream` weiter, aber verdoppele dabei Prozentzeichen,
wenn sie am Anfang einer Zeile stehen.

Ich habe einen entsprechenden Unit Test in der Datei `t_marked-files-out.cpp`
erstellt. Die Unit Tests werden im Hauptprogramm aufgerufen:

```c++
int main() {
	// run tests
}
```

Die eigentliche Test-Funktion lenkt die Ausgabe in eine Zeichenkette um:

```c++
#include <cassert>
#include <sstream>

#include "marked-files-out.h"

static void test_ostream(const std::string& in, const std::string& expected) {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << in;
	assert(out.str() == expected);
}

// test functions
// ...
```

Ein einfacher Test kopiert die Eingabe in die Ausgabe:

```c++
// ...
// test functions

static inline void simple_test() {
	test_ostream("abc\ndef\n", "abc\ndef\n");
}
// ...
	// run tests
	simple_test();
// ...
```

Ein Prozentzeichen am Anfang einer Zeile wird verdoppelt:

```c++
// ...
// test functions

static inline void escape_single_percent() {
	test_ostream("abc\n%def\n", "abc\n%%def\n");
}
// ...
	// run tests
	escape_single_percent();
// ...
```

Auch wenn mehrere Prozentzeichen am Anfang einer Zeile stehen, kommt ein
weiteres hinzu:

```c++
// ...
// test functions

static inline void escape_double_percent() {
	test_ostream("abc\n%%def\n", "abc\n%%%def\n");
}
// ...
	// run tests
	escape_double_percent();
// ...
```

Wenn ein Prozentzeichen nicht am Anfang einer Zeile steht, wird es nicht
verdoppelt:

```c++
// ...
// test functions

static inline void dont_escape_inner_percent() {
	test_ostream("a%c", "a%c");
}

// ...
	// run tests
	dont_escape_inner_percent();
// ...
```

Auch wenn ein Prozentzeichen ganz am Anfang der Datei steht, muss es gedoppelt
werden:

```c++
// ...
// test functions

static inline void escape_first_percent() {
	test_ostream("%abc", "%%abc");
}

// ...
	// run tests
	escape_first_percent();
// ...
```

## Die Umsetzung

In der Header-Datei deklariere ich die neue Klasse
`marked_files::basic_ostream`, welche nach außen hin ein `std::basic_ostream`
ist. Sie ist jedoch gleichzeitig ein `std::basic_streambuf`, da wir dessen
Methode `overflow` überschreiben müssen.

Wie bei den Standard `iostream` Klassen gibt es auch die Spezialisierung
`marked_files::ostream`. Dieses Vorgehen ist etwas umständlich, ermöglicht es
jedoch `streams` mit unterschiedlichen Basis-Typen zu verwenden.

Den Trick mit der privaten Basis-Klasse habe ich aus einem Stack Overflow
Post gelernt [2]. Hier ist die Header-Datei `marked-files-out.h`

```c++
#pragma once

#include <iostream>

namespace marked_files {
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_ostream:
		private std::basic_streambuf<CharT, Traits>,
		public std::basic_ostream<CharT, Traits>
	{
			std::basic_ostream<CharT, Traits>& forward_;

		public:
			basic_ostream(std::basic_ostream<CharT, Traits>& forward):
				std::ostream { this }, forward_ { forward }
			{ }
	};

	using ostream = basic_ostream<char>;
};
```

`forward_` ist eine Referenz auf den Stream, an den die Zeichen weitergesendet
werden. In `last_` wird das letzte Zeichen zwischengespeichert, um zu erkennen,
ob das aktuelle Zeichen am Anfang einer Zeile steht:

```c++
// ...
			std::basic_ostream<CharT, Traits>& forward_;
			CharT last_ { '\n' };
// ...
```

Die private Basis-Klasse `std::basic_streambuf` muss vor `std::basic_ostream`
stehen.  Denn wir müssen sie vor `std::basic_ostream` initialisieren.
`std::basic_ostream` erhält den initialisierten `std::basic_streambuf` als
Argument im Konstruktor.

Die eigentliche Logik findet in der Methode `overflow` statt:

```c++
// ...
			CharT last_ { '\n' };

			Traits::int_type forward_status() {
				return forward_ ? Traits::to_int_type(0) : Traits::eof();
			}

			Traits::int_type overflow(Traits::int_type ch) override
			{
				if (Traits::eq_int_type(ch, Traits::eof())) { return ch; }
				CharT c { Traits::to_char_type(ch) };
				if (c == '%' && last_ == '\n') { forward_.put(c); }
				forward_.put(c);
				last_ = c;
				return forward_status();
			}
// ...
```

Hier prüfen wir, ob das aktuelle Zeichen das Prozentzeichen ist und ob das
Zeichen davor ein Zeilenumbruch war. Da wir `last_` im `'\n'` initialisiert
haben, betrachten wir auch das erste Zeichen als Zeichen am Beginn einer neuen
Zeile. In diesem Fall wird das Zeichen zweimal ausgegeben.


## Warum funktioniert das?

Der `std::basic_streambuf` verwaltet einen Puffer-Speicher, in dem Zeichen
gesammelt werden, bevor sie weiter verarbeitet werden. Doch davon ist in
der Implementierung nichts zu sehen.

Immer wenn der Puffer voll ist, wird die Methode `overflow` mit dem nächsten
auszugebenden Zeichen aufgerufen. Da es in unserem Fall gar keinen Puffer gibt,
wird diese Methode bei jedem auszugebenden Zeichen aufgerufen. Das machen wir
uns zu Nutze, um jedes Zeichen einzeln zu behandeln.

Als Rückgabe-Wert von `overflow` ist nur `Traits::eof()` interessant.  Dieser
Wert signalisiert einen Fehler. Wir verwenden die `Traits`-Klasse, um zwischen
dem `Traits::int_type` Argument und Zeichen vom Typ `Traits::char_type` hin
und her zu rechnen. Das garantiert, dass wir diese Klasse auch für andere
Basis-Typen verwenden können, nicht nur für `char`.


## Kommandos integrieren

Wir haben die Klasse geschrieben, um Kommandos integrieren zu können. Dies
wird zum Beispiel benutzt, um mehrere Dateien in einem Archiv zusammenzufassen.
Oder um Prüfsummen über den Datei-Inhalt zu berechnen und hinzuzufügen.

Fügen wir in `t_marked-files-out.cpp` einen einfachen Unit Test hinzu. Dieser Test
demonstriert ebenfalls, wie wir Kommandos verwenden können:

```c++
// ...
// test functions

static inline void add_simple_command() {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << "abc";
	escaped.open_command("test");
	escaped << "xyz";
	escaped.close_command();
	assert(out.str() == "abc\n%test xyz\n");
}

// ...
	// run tests
	add_simple_command();
// ...
```

Zeilenumbrüche sind in Kommandos nicht erlaubt:

```c++
// ...
// test functions

static inline void no_nl_in_commands() {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << "abc";
	escaped.open_command("test");
	escaped << "x\nyz";
	assert(! escaped);
	assert(out.str() == "abc\n%test x");
}

// ...
	// run tests
	no_nl_in_commands();
// ...
```

Fügen wir passende Methoden in `marked-files-out.h` hinzu. Dabei halten wir in
dem zusätzlichen Flag `in_command_` fest, ob wir gerade im Kommando-Modus sind.
In diesem Modus escapen wir keine Prozentzeichen und erlauben keinen
Zeilenumbruch:

```c++
// ...
			CharT last_ { '\n' };
			bool in_command_ { false };
// ...
			Traits::int_type overflow(Traits::int_type ch) override
// ...
			{
// ...
				CharT c { Traits::to_char_type(ch) };
				if (in_command_) {
					if (c == '\n') { return Traits::eof(); }
					forward_.put(c); last_ = c;
					return forward_status();
				}
// ...
			}
// ...
		public:
			Traits::int_type open_command(const std::string& name) {
				if (last_ != '\n') { this->put('\n'); }
				in_command_ = true;
				*this << '%' << name << ' ';
				return forward_status();
			}

			Traits::int_type close_command() { 
				if (! in_command_) { return Traits::eof(); }
				in_command_ = false;
				this->put('\n');
				return forward_status();
			}
// ...
```

Da wir `last_` auf ein Leerzeichen setzen, verdoppelt der Stream das
Prozentzeichen nicht. Es wird als Markierung für das Kommando angegeben.
Die Argumente des Kommandos dürfen folgen. Wir dürfen jedoch keinen
Zeilenumbruch senden. Dieser darf im Kommando-Namen und den Argumenten nicht
vorkommen.

[1] Lattice C++ für den Amiga, Ralph Babel, Amiga Magazin 4/1989, Seite 150,
    Markt & Technik Verlag, München

[2] How to inherit from std::ostream?, Antwort von Ben und aSemy,
	 Stack Overflow, https://stackoverflow.com/questions/772355

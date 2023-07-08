# Pikafish Proxy

a Chinese Chess Engine Wrapper for XQWizard to run Pikafish

[Pikafish](https://github.com/official-pikafish/Pikafish) is derived from [Stockfish](https://github.com/official-stockfish/Stockfish) without engine protocol modification, which means Pikafish cannot be directly loaded by [XQWizard](https://www.xqbase.com/xqwizard/xqwizard.htm).

**Pikafish Proxy** has two features:

1. Protocol change from Stockfish (UCI) to ElephantEye (UCCI)
2. ElephantEye's Book move (Pikafish doesn't contain book so it always gives C2.5 at start position)

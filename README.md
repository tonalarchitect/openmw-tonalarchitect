# TonalArchitect - AI-Enhanced Dialogue for OpenMW

## Overview
TonalArchitect is a fork of OpenMW that enhances NPC dialogue with AI capabilities, allowing for more dynamic and contextual conversations in The Elder Scrolls III: Morrowind. This project adds a new layer of depth to character interactions by integrating modern AI language models while maintaining the lore-friendly atmosphere of the original game.

## Features
- **AI-Enhanced Dialogue**: Engage in dynamic conversations with NPCs using advanced AI language models
- **Context-Aware Responses**: NPCs respond based on their:
  - Race and class
  - Current location
  - Disposition toward the player
  - Previous dialogue history
- **Lore-Friendly**: AI responses are constrained to maintain consistency with Elder Scrolls lore
- **Original Game Features**: All original OpenMW features remain intact and functional

## Requirements
- Base Morrowind game files
- OpenMW dependencies
- Anthropic API key for Claude AI integration
- CMake 3.16.0 or higher
- Boost libraries
- OpenSceneGraph
- SDL2
- OpenAL
- Other standard OpenMW dependencies

## Building from Source

### Linux
```bash
git clone https://github.com/tonalarchitect/openmw.git
cd openmw
mkdir build
cd build
cmake ..
make
```

### Windows
Coming soon - Windows build instructions and precompiled binaries will be available in future releases.

## Configuration
1. Launch OpenMW
2. Go to Options
3. Enter your Anthropic API key in the designated field Under Prefs in the AI Chat Anthropic API Key field
4. Save settings and launch the game

## Usage
1. Start a conversation with any NPC
2. Use the new "Chat" button in the dialogue window to engage in AI-enhanced dialogue
3. The AI will respond based on the NPC's characteristics and current context

## Current Limitations
- Requires an active internet connection for AI features (local support is planned)
- API key required for functionality
- Responses may have slight delays depending on network conditions
- Currently optimized for English language only

## Contributing
We welcome contributions! Please feel free to:
- Submit bug reports
- Propose new features
- Submit pull requests
- Improve documentation
to tonalarchitect@proton.me

## Legal
- This project is a fork of OpenMW and maintains its original GPL3 license
- The AI integration components are subject to Anthropic's terms of service
- Morrowind® is a registered trademark of ZeniMax Media Inc.

## Credits
- OpenMW Team for the amazing base engine
- Anthropic for Claude AI integration and all code generation for this project.
- All contributors to this fork

## Roadmap
- [ ] Windows precompiled binaries
- [ ] Response caching system
- [ ] Offline fallback mode
- [ ] Multi-language support
- [ ] Performance optimizations
- [ ] Additional AI model options

## Support
- GitHub Issues: [Report bugs or suggest features](https://github.com/tonalarchitect/openmw/issues)
- Email: tonalarchitect@proton.me

## License
This project is licensed under GPL3, maintaining OpenMW's original licensing terms.

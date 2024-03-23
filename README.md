# ArenaGame
This is the main game repository.

## Commit Tags
- [Code]: Changes in the source code.
- [Design]: Game design changes in the editor.
- [BP]: Blueprint changes in the editor.
- [Art]: Model, VFX, Material, etc. imports and changes in the editor.
- [Sound]: Soundtrack, SFX imports, and changes in the editor.
- [UI]: UI/UX changes.
- [Fix]: When you fix something.
- [Config]: Config file changes

## Branching
- master: Main development environment
 
For the ones below, you should have a separate branch for each issue.

Follow this Naming: category/IssieID-Name
Example: feature/27-turret-placement

- stable: Production-ready versions  -> stable/v1.4, etc.
- features: Separate feature branches for each important feature
- bugs: Create a new branch for major bugs and work on them separately
- hotfixes: Work on hotfixes in this kind of branch

## Version Labels
[Major].[Minor].[Revision].[Package]

i.e. Version: 1.0.15.2

- Major build number: This indicates a major milestone in the game, increment this when going from beta to release, from release to major updates.
- Minor build number: Used for feature updates, large bug fixes, etc.
- Revision: Minor alterations on existing features, small bug fixes, etc.
- Package: Your code stays the same, external library changes, or asset file updates.

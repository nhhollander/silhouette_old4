# Scene file format

Scene files contain a list of entities, and how they should be constructed.
These files are JSON files which contain the following basic structure, as well
as a number of entity-specific attributes.

## Basic File Format

Below is a basic example of a scene file.  It describes an entity of type `sample_entity_type` named `sample_entity`.  The `pos`, `rot`, and `scale`
attributes are optional, and will default to zero (or 1 in the case of `scale`)
if omitted.

When parsing the file, the `type` is used to determine how the entity should be
constructed.  Entity constructors for any user-specified `type` can be
registered by calling `Scene::register_constructor()`.

```json
{
    "entities": [
        {
            "name": "sample_entity",
            "type": "sample_entity_type",
            "pos": {
                "x": 1.35,
                "y": 2.11,
                "z": 6.21
            },
            "rot": {
                "x": 3.14159,
                "y": 0.0,
                "z": 0.0
            },
            "scale": {
                "x": 1.0,
                "y": 1.0,
                "z": 1.0
            }
        }
    ]
}
```

## Static Prop entities

Entities of the type `static_prop` will construct a new
`se::entity::StaticProp`, and take an additional two parameters, `geometry` and
`texture`.  If either of these values are missing, the entity will not be
constructed.  The values should corespond with models and texture files located
in the `models` and `textures` subdirectory of the `appdata` folder
respectively.

Below is an example entity entry:

```json
{
    "name": "sample_static_prop",
    "type": "static_prop",
    "geometry": "test_geometry",
    "texture": "test_texture"
}
```
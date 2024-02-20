// To be used with Binary File Viewer extension
registerFileType((fileExt, filePath, fileData) => {
	// Check for the right file extension
	return (fileExt === 'toy');
});

registerParser(() => {
	addStandardHeader();

	showUnused = true;

	i = 0;
	while (!endOfFile()) { // 88 bytes per entity data struct
		i++
		read(4);
		addRow('Entity', i);
		tileEntityId = getSignedNumberValue();
		if (showUnused || tileEntityId > -1) {
			addRow('Tile.entityId', tileEntityId);
			read(4);
			addRow('Tile.tilePos.x', getFloatNumberValue());
			read(4);
			addRow('Tile.tilePos.y', getFloatNumberValue());
			read(4);
			addRow('Tile.tileId', getSignedNumberValue());
		}

		read(4);
		colliderEntityId = getSignedNumberValue();
		if (showUnused || colliderEntityId > -1) {
			addRow('Collider.entityId', colliderEntityId);
			read(4);
			addRow('Collider.type', getNumberValue(), 'TYPE_CIRCLE, TYPE_RECTANGLE');
			read(4)
			addRow('Collider.shape.', getFloatNumberValue(), 'union of Vector2 size, float radius');
			read(4)
			addRow('Collider.shape.', getFloatNumberValue(), 'union of Vector2 size, float radius');
		}

		read(4)
		posEntityId = getSignedNumberValue();
		if (showUnused || posEntityId > -1) {
			addRow('Position.entityId', posEntityId);
			read(4);
			addRow('Position.pos.x', getFloatNumberValue());
			read(4);
			addRow('Position.pos.y', getFloatNumberValue());
		}

		read(4)
		velEntityId = getSignedNumberValue();
		if (showUnused || velEntityId > -1) {
			addRow('Velocity.entityId', posEntityId);
			read(4);
			addRow('Velocity.vel.x', getFloatNumberValue());
			read(4);
			addRow('Velocity.vel.y', getFloatNumberValue());
		}


	}
})
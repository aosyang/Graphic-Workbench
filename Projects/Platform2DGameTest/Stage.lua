Stage = {
	GameWorldCommon = {
		TileTypes = {
			-- { Name, Usage, Texture }
			{ "StoneWall", "SOLID", "Data/Fieldstone.tga" },
			{ "Terrain", "SOLID", "Data/Terrain.tga" },
			{ "Ladder", "LADDER", "Data/Grid_Ladders.tga" },
		},
		-- Layers = 3,
		Geometries = {
			-- { LayerID, vMinX, vMaxX, vMinY, vMaxY, TileTypeName }
			{ 0, -5.0, 5.0, 0.0, 2.0, "StoneWall" },
			{ 0, -3.0, 2.0, -7.0, -6.0, "StoneWall" },
			{ 0, -2.0, 1.0, -6.0, -5.0, "StoneWall" },
			{ 0, -1.0, 0.0, -5.0, -4.0, "StoneWall" },

			{ 0, -10.0, -8.0, -7.0, -4.0, "StoneWall" },
			{ 0, -7.0, -5.0, -3.0, -1.0, "StoneWall" },

			-- Top
			{ 0, -11.0, 11.0, 7.0, 8.0, "StoneWall" },

			-- Left
			{ 0, -11.0, -10.0, -7.0, 7.0, "StoneWall" },

			-- Right
			{ 0, 10.0, 11.0, -7.0, 7.0, "StoneWall" },

			-- Bottom
			{ 0, -11.0, 11.0, -9.0, -7.0, "Terrain" },

			-- Stairs

			--{ 0, 0.0, 0.0, 1.0, 1.0, "Data/Default.tga" },
			{ 1, 6.0, 10.0, 0.0, 3.0, "Terrain" },

			-- Ladders
			{ 0, -8, -7, -7, 0, "Ladder" },
			{ 0, -6, -5, -1, 3, "Ladder" },
		},
	},

	GameWorld0 = {
		TileTypes = {
			-- { Name, Usage, Texture }
			{ "Ladder", "SOLID", "Data/Fieldstone.tga" },
		},
	},

	GameWorld1 = {
	},

	StartPoint = { 5.0, 10.6, 15.0 },

}

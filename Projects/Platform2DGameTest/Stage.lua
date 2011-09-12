Stage = {
	Layers = 3,
	Geometries = {
		-- { LayerID, vMinX, vMaxX, vMinY, vMaxY, TextureName }
		{ 0, -5.0, 5.0, 0.0, 2.0, "Data/Fieldstone.tga" },
		{ 0, -1.0, 2.0, -7.0, -6.0, "Data/Fieldstone.tga" },
		{ 0, -1.0, 1.0, -6.0, -5.0, "Data/Fieldstone.tga" },
		{ 0, -1.0, 0.0, -5.0, -4.0, "Data/Fieldstone.tga" },

		{ 0, -10.0, -8.0, -7.0, -4.0, "Data/Fieldstone.tga" },
		{ 0, -7.0, -5.0, -3.0, -1.0, "Data/Fieldstone.tga" },
		
		-- Top
		{ 0, -11.0, 11.0, 7.0, 8.0, "Data/Fieldstone.tga" },

		-- Left
		{ 0, -11.0, -10.0, -7.0, 7.0, "Data/Fieldstone.tga" },

		-- Right
		{ 0, 10.0, 11.0, -7.0, 7.0, "Data/Fieldstone.tga" },

		-- Bottom
		{ 0, -11.0, 11.0, -9.0, -7.0, "Data/Terrain.tga" },

		-- Stairs

		--{ 0, 0.0, 0.0, 1.0, 1.0, "Data/Default.tga" },
		{ 1, 6.0, 10.0, 0.0, 3.0, "Data/Terrain.tga" },
	},
	StartPoint = { 5.0, 10.6, 15.0 }
}

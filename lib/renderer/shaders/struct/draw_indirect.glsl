const uint DRAW_INDIRECT_COUNT_OFFSET = 0u;
const uint DRAW_INDIRECT_COMMANDS_OFFSET = 16u;
const uint DRAW_INDIRECT_RECORD_SIZE = 32u;

struct DrawIndirectCommand
{
	uint vertexCount;
	uint instanceCount;
	uint firstVertex;
	uint baseInstance;
};

struct DrawIndirectRecord
{
	DrawIndirectCommand command;
	uint idModel;
	uint padding0;
	uint padding1;
	uint padding2;
};

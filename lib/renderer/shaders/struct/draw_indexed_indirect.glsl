const uint DRAW_INDEXED_INDIRECT_COUNT_OFFSET = 0u;
const uint DRAW_INDEXED_INDIRECT_COMMANDS_OFFSET = 16u;
const uint DRAW_INDEXED_INDIRECT_RECORD_SIZE = 32u;

struct DrawIndexedIndirectCommand
{
	uint indexCount;
	uint instanceCount;
	uint firstIndex;
	int baseVertex;
	uint baseInstance;
};

struct DrawIndexedIndirectRecord
{
	DrawIndexedIndirectCommand command;
	uint idModel;
	uint padding0;
	uint padding1;
};

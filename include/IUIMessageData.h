namespace Subtitle
{
	enum MessageType : uint32_t
	{
		kShowMessage = 1,
		kShowSubtitle = 5,
		kHideSubtitle = 6,
		kShowArrowCount = 7,
	};

	struct IUIMessageData
	{
		uint32_t unk00;        // 00
		uint32_t unk04;        // 04
		uint32_t unk08;        // 08
		uint32_t unk0B;        // 0B
		MessageType type;      // 10
		uint32_t unk14;        // 14
		RE::BSString payload;  // 18
	};
}

struct InputData
{
	// �����x
    float3 accel;
    // �X�P�[��
    float scaleAce;
	// �����l
    float3 s_color;
    float s_scale;
    float3 s_rotation;
    float s_alpha;
	// �ŏI�l
    float3 e_color;
    float e_scale;
    float3 e_rotation;
    float e_alpha;
	// �I���t���[��
    int num_frame;
    //float3 pad;
};
struct OutputData
{
    float3 position;
    float scale;
	
    float3 rotation;
	//�r���{�[�h���邩�ǂ���
    uint billboardActive;
	
    float4 color;
	
    float3 velocity;
    uint frame;
	//���S�t���O
    uint isDead;
    float scaleVel;

    float2 pad;
};

//���̓f�[�^
StructuredBuffer<InputData> g_inputData : register(t0);
StructuredBuffer<OutputData> g_inputData2 : register(t1);

//�o�̓f�[�^
RWStructuredBuffer<OutputData> g_outputData : register(u0);

//[RootSignature(RS)]
[numthreads(30, 1, 1)]
void CSmain(uint3 gID : SV_DispatchThreadID)
{
    InputData input = g_inputData[gID.x];
    OutputData input2 = g_inputData2[gID.x];
    float3 velocity = input2.velocity;
    			// �o�߃t���[�������J�E���g
    input2.frame++;
			// �i�s�x��0�`1�͈̔͂Ɋ��Z
    const float f = (float) input.num_frame / input2.frame;

			// ���x�ɉ����x�����Z
    velocity += input.accel;
    velocity = normalize(velocity);
			// ���x�ɂ��ړ�
    input2.position += velocity;
    input2.velocity = velocity;

			// �J���[�̐��`���			//�A���t�@�l�̐��`���
    input2.color = float4(input.s_color + (input.e_color - input.s_color) / f, input.s_alpha + (input.e_alpha - input.s_alpha) / f);

			//�X�P�[�����x�ɉ����x�����Z
    input2.scaleVel += input.scaleAce;

			// �X�P�[���ɑ��x�����Z
    input2.scale += input2.scaleVel;

			// ��]�̐��`���
    input2.rotation = float4(input.s_rotation + (input.e_rotation - input.s_rotation) / f, 0);



	
	//�o�̓f�[�^
    OutputData output = input2;
    output.isDead = step(input.num_frame, input2.frame);
    //output.frame = input2.frame;
    output.pad = float2(0, 0);
    g_outputData[gID.x] = output;
}
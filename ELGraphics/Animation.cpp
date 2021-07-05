#include "Animation.hpp"
#include "Skeleton.hpp"

Animation* Animation::FromData(const Buffer<byte>& data)
{
	if (data[0] == ASSET_ANIMATION)
	{
		Animation* animation = new Animation();
		ByteReader reader(data);
		animation->Read(reader);
		return animation;
	}

	return nullptr;
}

void Animation::Evaluate(Buffer<Matrix4>& skinningMatrices, const Skeleton& skeleton, float time) const
{
	skinningMatrices.SetSize(skeleton.GetJointCount());

	for (auto it = skeleton.FirstListElement(); it; ++it)
	{
		const Joint& j = *it;

		auto translationTrack = _translationTracks.TryGet(j.name);
		auto rotationTrack = _rotationTracks.TryGet(j.name);
		auto scalingTrack = _scalingTracks.TryGet(j.name);

		Vector3 translation, scale;
		Quaternion rotation;

		if (translationTrack)
			translationTrack->Evaluate(translation, time);

		if (rotationTrack)
			rotationTrack->Evaluate(rotation, time);

		if (scalingTrack)
			scalingTrack->Evaluate(scale, time);

		skinningMatrices[j.GetID()] = Matrix4::Transformation(translation, rotation, scale);
	}
}

void Animation::Read(ByteReader& iterator)
{
	iterator.Read_byte();

	uint32 tTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < tTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _translationTracks[iterator.Read<String>()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 rTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < rTrackCount; ++i)
	{
		AnimationTrack<Quaternion>& track = _rotationTracks[iterator.Read<String>()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Quaternion value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 sTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < sTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _scalingTracks[iterator.Read<String>()];
		
		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}
}

void Animation::Write(ByteWriter &iterator) const
{
	Debug::PrintLine("_________________\nSaving animation...");

	iterator.Write_byte(ASSET_ANIMATION);

	Debug::PrintLine("TRANSLATION TRACKS:\n{");

	iterator.Write_uint32((uint32)_translationTracks.GetSize());
	for (const Pair<const String, AnimationTrack<Vector3>>& kv : _translationTracks)
	{
		Debug::PrintLine(CSTR("\t\"", kv.first, "\":\n\t{"));

		iterator.Write_cstr(kv.first.begin());
		
		auto keyframes = kv.second.GetKeyframes();
		
		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nROTATION TRACKS:\n{");

	iterator.Write_uint32((uint32)_rotationTracks.GetSize());
	for (const Pair<const String, AnimationTrack<Quaternion>>& kv : _rotationTracks)
	{
		Debug::PrintLine(CSTR("\t\"", kv.first, "\":\n\t{"));

		iterator.Write_cstr(kv.first.begin());

		auto keyframes = kv.second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value.ToEulerYXZ()));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.ToEulerYXZ().Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nSCALING TRACKS:\n{");

	iterator.Write_uint32((uint32)_scalingTracks.GetSize());
	for (const Pair<const String, AnimationTrack<Vector3>>& kv : _scalingTracks)
	{
		Debug::PrintLine(CSTR("\t\"", kv.first, "\":\n\t{"));

		iterator.Write_cstr(kv.first.begin());

		auto keyframes = kv.second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}");
}

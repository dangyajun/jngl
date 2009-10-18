/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNAL.

JNAL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNAL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNAL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "jnal.hpp"
#include "debug.hpp"

#include <AL/al.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>
#include <cstdio>
#include <vector>
#include <stdexcept>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace jnal
{
	class Sound : boost::noncopyable {
	public:
		Sound(ALenum format, std::vector<char>& bufferData, ALsizei freq) : source_(0)
		{
			alGenBuffers(1, &buffer_);
			alGenSources(1, &source_);
			alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
			alSource3f(source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
			alBufferData(buffer_, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);
			alSourcei(source_, AL_BUFFER, buffer_);
			alSourcePlay(source_);
		}
		~Sound()
		{
			Debug("freeing sound buffer ... ");
			alSourceStop(source_);
			alSourceUnqueueBuffers(source_, 1, &buffer_);
			alDeleteSources(1, &source_);
			alDeleteBuffers(1, &buffer_);
			Debug("OK\n");
		}
		bool IsPlaying()
		{
			ALint state;
			alGetSourcei(source_, AL_SOURCE_STATE, &state);
			return state == AL_PLAYING;
		}
		bool Stopped()
		{
			ALint state;
			alGetSourcei(source_, AL_SOURCE_STATE, &state);
			return state == AL_STOPPED;
		}
	private:
		ALuint buffer_;
		ALuint source_;
	};

	class JNAL : boost::noncopyable {
	public:
		JNAL()
		{
			alutInit(0, 0);
		}
		~JNAL()
		{
			alutExit();
		}
		void Play(boost::shared_ptr<Sound> sound)
		{
			std::vector<boost::shared_ptr<Sound> >::iterator end = sounds_.end();
			for(std::vector<boost::shared_ptr<Sound> >::iterator it = sounds_.begin(); it != end; ++it)
			{
				if((*it)->Stopped())
				{
					it = sounds_.erase(it);
				}
			}
			sounds_.push_back(boost::shared_ptr<Sound>(sound));
		}
		void Stop(boost::shared_ptr<Sound> sound)
		{
			std::vector<boost::shared_ptr<Sound> >::iterator i;
			if((i = std::find(sounds_.begin(), sounds_.end(), sound)) != sounds_.end()) // sound hasn't been loaded yet?
			{
				sounds_.erase(i);
			}
		}
	private:
		std::vector<boost::shared_ptr<Sound> > sounds_;
	};

	JNAL& GetJNAL()
	{
		static JNAL jnal_;
		return jnal_;
	}

	class SoundFile : boost::noncopyable {
	public:
		SoundFile(const std::string& filename) : sound_((Sound*)0)
		{
			Debug("Decoding "); Debug(filename); Debug(" ... ");
			// based on http://www.gamedev.net/reference/articles/article2031.asp
			FILE* f = fopen(filename.c_str(), "rb");
			if(!f)
			{
				throw std::runtime_error("Could not open OGG file.");
			}

			OggVorbis_File oggFile;
			if(ov_open(f, &oggFile, 0, 0) != 0)
			{
				throw std::runtime_error("Could not open OGG file.");
			}

			vorbis_info* pInfo;
			pInfo = ov_info(&oggFile, -1);
			if(pInfo->channels == 1)
			{
				format = AL_FORMAT_MONO16;
			}
			else
			{
				format = AL_FORMAT_STEREO16;
			}
			freq = pInfo->rate;

			const int bufferSize = 32768;
			char array[bufferSize]; // 32 KB buffers
			const int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
			int bitStream;
			int bytes;
			do
			{
				bytes = ov_read(&oggFile, array, bufferSize, endian, 2, 1, &bitStream);

				if (bytes < 0)
				{
					ov_clear(&oggFile);
					throw std::runtime_error("Error decoding OGG file.");
				}

				buffer_.insert(buffer_.end(), array, array + bytes);
			}
			while(bytes > 0);

			ov_clear(&oggFile);
			Debug("OK\n");
		}
		void Play()
		{
			sound_.reset(new Sound(format, buffer_, freq));
			GetJNAL().Play(sound_);
		}
		void Stop()
		{
			if(sound_)
			{
				GetJNAL().Stop(sound_);
				sound_.reset((Sound*)0);
			}
		}
		bool IsPlaying()
		{
			if(sound_)
			{
				return sound_->IsPlaying();
			}
			return false;
		}
	private:
		boost::shared_ptr<Sound> sound_;
		ALint state;
		ALenum format;
		ALsizei freq;
		std::vector<char> buffer_;
	};

	std::map<std::string, boost::shared_ptr<SoundFile> > sounds;

	SoundFile& GetSoundFile(const std::string& filename)
	{
		GetJNAL();
		std::map<std::string, boost::shared_ptr<SoundFile> >::iterator i;
		if((i = sounds.find(filename)) == sounds.end()) // sound hasn't been loaded yet?
		{
			sounds[filename].reset(new SoundFile(filename));
			return *(sounds[filename]);
		}
		return *(i->second);
	}

	void Play(const std::string& filename)
	{
		GetSoundFile(filename).Play();
	}

	void Stop(const std::string& filename)
	{
		GetSoundFile(filename).Stop();
	}

	void Load(const std::string& filename)
	{
		GetSoundFile(filename);
	}

	bool IsPlaying(const std::string& filename)
	{
		return GetSoundFile(filename).IsPlaying();
	}
};
#include "core.hpp"
#include "core.using"

namespace core {

void checkVersionImpl (const char *requirer, const char *requiree, Version required, Version available) noexcept {
  if (required.major != available.major || required.minor > available.minor) {
    fprintf(stderr, "%s depends on %s %u.%u or compatible, but %u.%u is present\n", requirer, requiree, required.major, required.minor, available.major, available.minor);
    fflush(stderr);
    dieHard();
  }
}

const Version VERSION{LIB_MAJ, LIB_MIN}; DEPENDENCIES;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#ifndef NDEBUG
namespace debug {

const char *const Stream::INDENT = "  ";

Stream::Stream () noexcept : handle(stderr), indentCount(0), atStartOfLine(true) {
}

Stream::Stream (const char *filename) noexcept : indentCount(0), atStartOfLine(true) {
  if (!filename) dieHard();

  handle = fopen(filename, "wb");
  if (!handle) {
    fprintf(stderr, "failed to open Stream to %s\n", filename);
    fflush(stderr);
    dieHard();
  }
}

Stream::~Stream () noexcept {
  if (handle != stderr) {
    fclose(handle);
  }
}

void Stream::enterScope () noexcept {
  ++indentCount;
}

void Stream::exitScope () noexcept {
  if (indentCount == 0) dieHard();
  --indentCount;
}

void Stream::startLine () noexcept {
  if (!atStartOfLine) {
    return;
  }

  for (iu i = 0; i < indentCount; ++i) {
    writeElement(INDENT);
  }
  atStartOfLine = false;
}

void Stream::endLine () noexcept {
  writeElement("\n");
  atStartOfLine = true;
}

void Stream::flush () noexcept {
  fflush(handle);
}

void Stream::writeElement (const char *value) noexcept {
  int r = fputs(value, handle);
  if (r < 0) dieHard("failed to write to stream\n");
}

void Stream::writeElement (const char8_t *value) noexcept {
  // We're not even going to think about converting the characters to the native
  // charenc. Instead, we make the value stand out (by wrapping it in the BOM) and
  // hope that the writing won't make the bytes unintelligible.
  const char *const bom = "\xEF\xBB\xBF";
  this->writeElement(bom);
  this->writeElement(reinterpret_cast<const char *>(value));
  this->writeElement(bom);
}

Logger::ScopeProxy::ScopeProxy (Logger &logger, const char *scopeName) noexcept :
  stream(logger.stream), scopeName(scopeName)
{
  if (!scopeName) dieHard();

  if (stream) {
    writeMarker(true);
    stream->enterScope();
  }
}

Logger::ScopeProxy::~ScopeProxy () noexcept {
  if (stream) {
    stream->exitScope();
    writeMarker(false);
  }
}

void Logger::ScopeProxy::writeMarker (bool entering) noexcept {
  stream->startLine();
  stream->writeElement(entering ? ">>" : "<<");
  stream->writeElement(scopeName);
  stream->endLine();
  stream->flush();
}

Logger::Logger () noexcept : stream(nullptr) {
}

void Logger::open (const std::shared_ptr<Stream> &stream) noexcept {
  this->stream = stream;
}

void Logger::close () noexcept {
  this->stream.reset();
}

Logger::ScopeProxy Logger::defineScope (const char *scopeName) noexcept {
  return ScopeProxy(*this, scopeName);
}

void Logger::writeElements () noexcept {
  // That's all, folks.
}

}
#endif

DC();

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
noreturn void dieHard () noexcept {
  volatile int i = 0;
  int x = 0 / (i - i);

  exit(EXIT_FAILURE + static_cast<int>(x - x));
}

noreturn void dieHard (const char *msg) noexcept {
  fputs(msg, stderr);
  fflush(stderr);
  dieHard();
}

void check (const bool &cond) noexcept {
  if (!cond) {
    dieHard("test failed\n");
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
const char *UException::what () const noexcept {
  return reinterpret_cast<const char *>(uWhat());
}

void buildExceptionMessagePart (const std::exception &exception, u8string &r_out) {
  u8string tmp(0, static_cast<char8_t>(0));
  const char8_t *subMsgBegin, *subMsgEnd;

  const std::exception *e = &exception;
  const UException *ue = dynamic_cast<const UException *>(e);
  if (ue) {
    subMsgBegin = ue->uWhat();
    subMsgEnd = subMsgBegin + std::char_traits<char8_t>::length(subMsgBegin);
  } else {
    subMsgBegin = reinterpret_cast<const char8_t *>(e->what());
    subMsgEnd = + subMsgBegin + std::char_traits<char>::length(e->what());
    tmp.append(subMsgBegin, subMsgEnd);
    subMsgBegin = &tmp[0];
    subMsgEnd = subMsgBegin + tmp.size();

    // We don't know anything about the native character set, so we'll just make
    // sure the octets aren't invalid UTF-8 and cross our fingers that something
    // sensible comes out.
    for (char8_t &c : tmp) {
      if (c < 32 || c >= 128) {
        c = U'?';
      }
    }
  }
  // TODO if the message doesn't look like a sentence (e.g. has no spaces in), stop here? (and hoist putting the . on the end)

  bool headIsCapitalisable = true;
  if (subMsgBegin != subMsgEnd && *subMsgBegin == U'_') {
    headIsCapitalisable = false;
    ++subMsgBegin;
  }

  if (r_out.empty()) {
    r_out.append(subMsgBegin, subMsgEnd);
    if (headIsCapitalisable && !r_out.empty()) {
      char8_t &c = r_out.front();
      // TODO do this properly (against the first character, according to the current locale)
      if (c < 128) {
        c = static_cast<char8_t>(std::toupper(c));
      }
    }
  } else {
    r_out.append(u8(": "));
    r_out.append(subMsgBegin, subMsgEnd);
  }

  try {
    std::rethrow_if_nested(exception);
  } catch (const std::exception &e) {
    buildExceptionMessagePart(e, r_out);
    return;
  } catch (...) {
    // Recurse no further.
  }
  r_out.append(u8("."));
  return;
}

u8string buildExceptionMessage (const std::exception &rootException) {
  u8string out;

  try {
    buildExceptionMessagePart(rootException, out);
  } catch (...) {
    out = u8("An error occurred (but further detail is unavailable, as an error occurred while building the message).");
  }

  return out;
}

PlainException::PlainException (const u8string &msg) : literalMsg(nullptr), composedMsg(new u8string(msg)) {
}

PlainException::PlainException (u8string &&msg) : literalMsg(nullptr), composedMsg(new u8string(move(msg))) {
}

PlainException::PlainException (const char8_t *msg) noexcept : literalMsg(msg), composedMsg(nullptr) {
}

PlainException PlainException::create (const char8_t *msgTemplate) {
  const char8_t *i = msgTemplate;
  char8_t c;
  while ((c = *(i++)) != 0) {
    if (c == '%') {
      --i;

      u8string msg(msgTemplate, i);
      interpolate(i, msg);
      return PlainException(move(msg));
    }
  }

  return PlainException(msgTemplate);
}

void PlainException::interpolate (const char8_t *msgTemplate, u8string &r_out) {
  char8_t c;
  while ((c = *(msgTemplate++)) != 0) {
    if (c == '%') {
      c = *msgTemplate;
      if (c != 0) {
        ++msgTemplate;
        switch (c) {
          case '%':
            r_out.push_back('%');
            break;
          default:
            break;
        }
      }
    } else {
      r_out.push_back(c);
    }
  }
}

const char8_t *PlainException::uWhat () const noexcept {
  return literalMsg ? literalMsg : composedMsg->c_str();
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}

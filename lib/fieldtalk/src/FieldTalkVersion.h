/**
 * @file FieldTalkVersion.h
 *
 * @if NOTICE
 *
 * Copyright (c) proconX Pty Ltd. All rights reserved.
 *
 * THIS IS PROPRIETARY SOFTWARE AND YOU NEED A LICENSE TO USE OR REDISTRIBUTE.
 *
 * THIS SOFTWARE IS PROVIDED BY PROCONX AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PROCONX OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @endif
 */


/* Version identifiers */
static char banner[] = "" /* No UNICODE here */
#ifdef EVAL_VERSION
                       "FieldTalk(tm) EVALUATION VERSION\n"
#else
                       "FieldTalk(tm)\n"
#endif
                       "Copyright (c) 2002-2012 proconX Pty Ltd. All rights reserved.";
#ifdef PACKAGE_VERSION
#if defined(_UNICODE)
#define PACKAGE_VERSION_STR2(arg) L#arg
#else
#define PACKAGE_VERSION_STR2(arg) #arg
#endif
#define PACKAGE_VERSION_STR(arg) PACKAGE_VERSION_STR2(arg)
static const TCHAR packageVersion[] = PACKAGE_VERSION_STR(PACKAGE_VERSION); ///< Package version
#else
#if defined(_UNICODE)
static const TCHAR packageVersion[] = L"2.6.8"; ///< Package version
#else
static const TCHAR packageVersion[] = "2.6.8"; ///< Package version
#endif
#endif


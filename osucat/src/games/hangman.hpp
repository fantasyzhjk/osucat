#pragma once

#define HANGMAN_MOD_E	0x01
#define	HANGMAN_MOD_H	0x02
#define	HANGMAN_MOD_D	0x04
#define HANGMAN_MOD_P	0x08
#define HANGMAN_MOD_I	0x10
#define HANGMAN_EXPIRATION_TIME	3600
#define HANGMAN_WORD_LIST 5000
#define HANGMAN_INITIAL_HP 7

static enum CuteEmojiType { Happy, Sad };
static string cuteEmoji(CuteEmojiType type) {
	static string happy_emojis[9] = { u8"owo", u8">w<", u8"(*^��^*)", u8"��~", "OwO", "owO", "Owo", u8"��~owo", u8"����~>w<" };
	static string sad_emojis[8] = { "qwq", u8"������", u8"555", u8"�޿�", u8"QwQ", u8"TAT", u8"T_T", u8"o(�i�n�i)o" };
	if (type == CuteEmojiType::Happy) {
		return happy_emojis[utils::randomNum(0, 9 - 1)];
	}
	else if (type == CuteEmojiType::Sad) {
		return sad_emojis[utils::randomNum(0, 8 - 1)];
	}
}

typedef struct Word {
	string str;
	int freq;
	int index;
};

class Hangman {
	friend class HangmanGame;
private:
	static enum WordLibOperation { GenerateRandomWord, FindWordByIndex, FindWordByStr };
	int HP;
	int64_t start_time;
	char mods;
	int64_t player_id;
	string player_nickname, guess_history;
	Word word;
	const static int number_of_words = 10000;

	static double scoring(Word word, string guess_history, char mods) {
		const double H = 1000000000.0;
		string list = "", list_E = "", list_history = "", list_historyE = "";
		bool uncompleted_play = false;
		for (int i = 0; i < word.str.length(); i++) {
			if (list.find(word.str[i]) == list.npos) {
				list += word.str[i];
				if (i != 1) list_E += word.str[i];
			}
			if (list_history.find(word.str[i]) == list_history.npos && guess_history.find(word.str[i]) != guess_history.npos) {
				list_history += word.str[i];
				if (i != 1) list_historyE += word.str[i];
			}//����Fail�Ʒ�ʱ�������ϵĴ���*/
			if (guess_history.find(word.str[i]) == guess_history.npos) uncompleted_play = true;
		}
		int d = list.length();
		int miss_count = guess_history.length() - d;
		double pp;
		double extra = 0;
		if (mods & HANGMAN_MOD_E) {
			if (uncompleted_play)
				pp = -57.0 * log(subStringSum(list_historyE, "", d - 1 + HANGMAN_INITIAL_HP + 1)) / d;
			else
				pp = -57.0 * log(subStringSum(list_E, "", d - 1 + miss_count)) / d;
		} else {
			if (uncompleted_play) 
				pp = -57.0 * log(subStringSum(list_history, "", d + HANGMAN_INITIAL_HP)) / d;
			else
				pp = -57.0 * log(subStringSum(list, "", d + miss_count)) / d;
		}
		if (mods & HANGMAN_MOD_H) {
			pp *= 1.12; extra += 4;
		}
		if (mods & HANGMAN_MOD_D) {
			pp *= 1.18; extra += 6;
		}
		if (mods & HANGMAN_MOD_I) {
			extra += 5;
		}
		double diff = -7.0 * log(1 - exp(-7000.1 * word.freq / H));
		if (!uncompleted_play) pp += diff;
		if (!uncompleted_play) pp += extra;
		return pp;
	}
	static double subStringSum(string list, string S, int n) {
		const static double letter_freqs[26] = { 8.06995892709077E-02, .012276878973113, 3.98002275400605E-02, 3.18491807915052E-02, 8.46011942639336E-02, 1.57221183079761E-02, 3.05829267822364E-02, 2.38788509869711E-02, 6.94961337472763E-02, 2.52608033321121E-03, 9.80222012251168E-03, 5.80677092372266E-02, 3.38289078719862E-02, 7.61616434305842E-02, 6.84805594657308E-02, 3.48187714122267E-02, 2.68677246636714E-03, 8.25957564421476E-02, 6.70343302673274E-02, 7.79935337485618E-02, 4.25191384330589E-02, 1.34274346465094E-02, 1.17498087763615E-02, 3.99802027291952E-03, 2.23812003059578E-02, 3.02101210333147E-03 };
		if (list == "") {
			double p = 0;
			for (int i = 0; i < S.length(); i++) {
				p += letter_freqs[S[i] - 'a'];
			}
			if (S.length() % 2) return -pow(1 - p, n);
			else return pow(1 - p, n);
		}
		else {
			string head = list.substr(0, 1);
			string tail = list.substr(1);
			return subStringSum(tail, head + S, n) + subStringSum(tail, S, n);
		}
	}
	static Word wordLibrary(WordLibOperation operation, int parameter) { // removed: dost, adv, les, spake
		static string word_lib[number_of_words] = { "the", "and", "that", "was", "his", "with", "for", "had", "you", "not", "her", "which", "have", "from", "this", "him", "but", "all", "she", "they",
"were", "are", "one", "their", "said", "them", "who", "would", "been", "will", "when", "there", "more", "out", "into", "any", "your", "what", "has", "man",
"could", "other", "than", "our", "some", "very", "time", "upon", "about", "may", "its", "only", "now", "like", "little", "then", "can", "should", "made", "did",
"such", "great", "before", "must", "two", "these", "see", "know", "over", "much", "down", "after", "first", "good", "men", "own", "never", "most", "old", "shall",
"day", "where", "those", "came", "come", "himself", "way", "work", "life", "without", "make", "well", "through", "being", "long", "say", "might", "how", "too", "even",
"def", "again", "many", "back", "here", "think", "every", "people", "went", "same", "last", "thought", "away", "under", "take", "found", "hand", "eyes", "still", "place",
"while", "just", "also", "young", "yet", "though", "against", "things", "get", "ever", "give", "god", "years", "off", "face", "nothing", "right", "once", "another", "left",
"part", "saw", "house", "world", "head", "three", "took", "new", "love", "always", "put", "night", "each", "king", "between", "tell", "mind", "heart", "few", "because",
"thing", "whom", "far", "seemed", "looked", "called", "whole", "set", "both", "got", "find", "done", "heard", "look", "name", "days", "told", "let", "lord", "country",
"asked", "going", "seen", "better", "having", "home", "knew", "side", "something", "moment", "father", "among", "course", "hands", "woman", "enough", "words", "mother", "soon", "full",
"end", "gave", "room", "almost", "small", "thou", "cannot", "water", "want", "however", "light", "quite", "brought", "nor", "word", "whose", "given", "door", "best", "turned",
"taken", "does", "use", "morning", "myself", "felt", "until", "since", "power", "themselves", "used", "rather", "began", "present", "voice", "others", "white", "works", "less", "money",
"next", "poor", "death", "stood", "form", "within", "together", "till", "large", "matter", "kind", "often", "certain", "herself", "year", "friend", "half", "order", "round", "true",
"anything", "keep", "sent", "wife", "means", "believe", "passed", "feet", "near", "public", "state", "son", "hundred", "children", "thus", "hope", "alone", "above", "case", "dear",
"thee", "says", "person", "high", "read", "city", "already", "received", "fact", "gone", "girl", "known", "hear", "times", "least", "perhaps", "sure", "indeed", "english", "open",
"body", "itself", "along", "land", "return", "leave", "air", "nature", "answered", "either", "law", "help", "lay", "point", "child", "letter", "four", "wish", "fire", "cried",
"women", "speak", "number", "therefore", "hour", "friends", "held", "free", "war", "during", "several", "business", "whether", "manner", "second", "reason", "replied", "united", "call", "general",
"why", "behind", "became", "john", "become", "dead", "earth", "boy", "lost", "forth", "thousand", "looking", "family", "soul", "feel", "coming", "spirit", "question", "care", "truth",
"ground", "really", "rest", "mean", "different", "making", "possible", "fell", "towards", "human", "kept", "short", "town", "following", "need", "cause", "met", "evening", "returned", "five",
"strong", "able", "french", "live", "lady", "subject", "answer", "sea", "fear", "understand", "hard", "terms", "doubt", "around", "ask", "arms", "turn", "sense", "seems", "black",
"bring", "followed", "beautiful", "close", "dark", "hold", "character", "sort", "sight", "ten", "show", "party", "fine", "ready", "story", "common", "book", "electronic", "talk", "account",
"mark", "interest", "written", "bed", "necessary", "age", "else", "force", "idea", "longer", "art", "spoke", "across", "brother", "early", "ought", "sometimes", "line", "saying", "table",
"appeared", "river", "continued", "eye", "sun", "information", "later", "everything", "reached", "suddenly", "past", "hours", "strange", "deep", "change", "miles", "feeling", "act", "meet", "paid",
"further", "purpose", "happy", "added", "seem", "taking", "blood", "rose", "south", "beyond", "cold", "neither", "forward", "view", "position", "sound", "none", "entered", "clear", "road",
"late", "stand", "suppose", "daughter", "real", "nearly", "mine", "laws", "knowledge", "comes", "toward", "bad", "cut", "copy", "husband", "six", "living", "peace", "low", "north",
"remember", "effect", "natural", "pretty", "fall", "fair", "service", "below", "except", "hair", "laid", "pass", "led", "copyright", "doing", "army", "run", "horse", "future", "opened",
"pleasure", "history", "west", "pay", "red", "hath", "note", "although", "wanted", "gold", "makes", "desire", "play", "master", "office", "tried", "front", "big", "lived", "certainly",
"wind", "receive", "attention", "government", "unto", "church", "strength", "length", "company", "placed", "paper", "letters", "probably", "glad", "important", "especially", "greater", "yourself", "fellow", "bear",
"opinion", "window", "ran", "faith", "ago", "agreement", "charge", "beauty", "lips", "remained", "arm", "latter", "duty", "send", "distance", "silence", "foot", "wild", "object", "die",
"save", "gentleman", "trees", "green", "trouble", "smile", "books", "wrong", "various", "sleep", "persons", "blockquote", "happened", "particular", "drew", "minutes", "hardly", "walked", "chief", "chance",
"according", "beginning", "action", "deal", "loved", "visit", "thinking", "follow", "standing", "knows", "try", "presence", "heavy", "sweet", "plain", "donations", "immediately", "wrote", "mouth", "rich",
"thoughts", "months", "afraid", "single", "joy", "enemy", "broken", "unless", "states", "ship", "condition", "carry", "exclaimed", "including", "filled", "seeing", "influence", "write", "boys", "appear",
"outside", "secret", "parts", "please", "appearance", "evil", "march", "george", "whatever", "slowly", "tears", "horses", "places", "caught", "stay", "instead", "struck", "blue", "impossible", "period",
"sister", "battle", "school", "raised", "occasion", "married", "former", "food", "youth", "learned", "merely", "reach", "system", "twenty", "dinner", "quiet", "easily", "moved", "afterwards", "giving",
"walk", "stopped", "laughed", "language", "expression", "week", "hall", "danger", "property", "wonder", "usual", "figure", "born", "court", "generally", "grew", "showed", "getting", "ancient", "respect",
"third", "worth", "simple", "tree", "leaving", "remain", "society", "fight", "wall", "result", "heaven", "started", "command", "tone", "regard", "expected", "mere", "month", "beside", "silent",
"perfect", "experience", "street", "writing", "goes", "circumstances", "entirely", "fresh", "duke", "covered", "bound", "east", "wood", "stone", "quickly", "notice", "bright", "boat", "noble", "meant",
"somewhat", "sudden", "value", "direction", "chair", "due", "support", "tom", "date", "waiting", "village", "lives", "reading", "agree", "lines", "considered", "field", "observed", "scarcely", "wished",
"wait", "greatest", "permission", "success", "piece", "british", "formed", "speaking", "trying", "conversation", "proper", "hill", "music", "opportunity", "afternoon", "cry", "cost", "allowed", "girls", "considerable",
"broke", "honour", "seven", "private", "sit", "news", "top", "scene", "discovered", "marriage", "step", "garden", "race", "begin", "per", "individual", "sitting", "learn", "political", "difficult",
"bit", "speech", "lie", "cast", "eat", "authority", "floor", "ill", "ways", "officers", "offered", "original", "happiness", "flowers", "produced", "summer", "provide", "study", "religion", "picture",
"walls", "personal", "watch", "pleased", "leaves", "declared", "hot", "understood", "effort", "prepared", "escape", "attempt", "supposed", "killed", "fast", "author", "brown", "determined", "pain", "spring",
"takes", "drawn", "soldiers", "houses", "beneath", "talking", "turning", "century", "steps", "intended", "soft", "straight", "matters", "likely", "corner", "trademark", "justice", "simply", "produce", "trust",
"appears", "laugh", "forget", "passage", "eight", "closed", "ourselves", "gives", "dress", "passing", "terrible", "required", "medium", "efforts", "sake", "breath", "wise", "ladies", "possession", "pleasant",
"perfectly", "memory", "usually", "grave", "fixed", "modern", "spot", "troops", "rise", "break", "fifty", "island", "meeting", "camp", "nation", "existence", "reply", "copies", "sky", "touch",
"equal", "fortune", "shore", "domain", "named", "situation", "looks", "promise", "orders", "degree", "middle", "winter", "plan", "spent", "allow", "pale", "conduct", "running", "religious", "surprise",
"minute", "roman", "cases", "shot", "lead", "move", "names", "stop", "higher", "threw", "worse", "built", "spoken", "glass", "board", "vain", "affairs", "instance", "safe", "loss",
"doctor", "offer", "class", "complete", "access", "lower", "repeated", "forms", "darkness", "military", "warm", "drink", "passion", "ones", "physical", "example", "ears", "questions", "start", "lying",
"smiled", "keeping", "spite", "shown", "directly", "james", "hart", "serious", "hat", "dog", "silver", "sufficient", "main", "mentioned", "servant", "pride", "crowd", "train", "wonderful", "moral",
"instant", "associated", "path", "greek", "meaning", "fit", "ordered", "lot", "proved", "obliged", "enter", "rule", "sword", "attack", "seat", "game", "health", "paragraph", "statement", "social",
"refund", "sorry", "courage", "members", "grace", "official", "dream", "worthy", "rock", "jack", "provided", "special", "shook", "request", "mighty", "glance", "heads", "movement", "fee", "share",
"expect", "dollars", "spread", "opposite", "glory", "twelve", "space", "engaged", "peter", "wine", "ordinary", "mountains", "taste", "iron", "distribute", "trade", "consider", "greatly", "accepted", "forced",
"advantage", "ideas", "decided", "using", "officer", "rate", "clothes", "sign", "feelings", "native", "promised", "judge", "difference", "working", "anxious", "marry", "captain", "finished", "extent", "watched",
"curious", "foreign", "besides", "method", "excellent", "confidence", "marked", "jesus", "exactly", "importance", "finally", "bill", "vast", "prove", "fancy", "quick", "yes", "sought", "prevent", "neck",
"hearts", "liberty", "interesting", "sides", "legal", "gentlemen", "dry", "serve", "aside", "pure", "concerning", "forgotten", "lose", "powers", "possessed", "thrown", "evidence", "distant", "michael", "progress",
"similar", "narrow", "altogether", "building", "page", "particularly", "knowing", "weeks", "settled", "holding", "mountain", "search", "sad", "sin", "lies", "proud", "pieces", "clearly", "price", "ships",
"thirty", "sick", "honest", "shut", "talked", "bank", "fate", "dropped", "judgment", "conditions", "accept", "hills", "removed", "forest", "measure", "species", "seek", "highest", "otherwise", "stream",
"honor", "carefully", "obtained", "ear", "bread", "bottom", "additional", "presented", "aid", "fingers", "remembered", "choose", "agreed", "animal", "events", "fully", "delight", "rights", "amount", "obtain",
"tax", "servants", "sons", "cross", "shoulders", "thick", "points", "stranger", "woods", "facts", "dare", "grow", "creature", "hung", "rain", "false", "tall", "gate", "nations", "created",
"refused", "quietly", "surface", "freely", "holy", "streets", "blow", "july", "regarded", "fashion", "report", "coast", "daily", "file", "shoulder", "surprised", "faces", "succeeded", "birds", "distribution",
"royal", "song", "wealth", "comfort", "failed", "freedom", "peculiar", "anyone", "advance", "gentle", "surely", "animals", "waited", "secure", "desired", "grass", "touched", "occupied", "draw", "stage",
"portion", "expressed", "opening", "june", "spirits", "fish", "tongue", "capital", "angry", "growing", "served", "carriage", "weather", "breast", "presently", "snow", "david", "papers", "necessity", "practice",
"claim", "hast", "education", "sharp", "prince", "permitted", "group", "enemies", "robert", "played", "throughout", "pity", "expense", "yours", "million", "add", "pray", "taught", "explained", "tired",
"leading", "kill", "shadow", "companion", "weight", "mass", "established", "suffered", "gray", "brave", "thin", "satisfied", "check", "virtue", "golden", "numerous", "frequently", "famous", "telling", "powerful",
"alive", "waters", "national", "weak", "divine", "material", "principal", "gathered", "suggested", "frank", "valley", "guess", "finding", "yellow", "heat", "remains", "bent", "seized", "guard", "equally",
"naturally", "box", "remarkable", "gods", "moon", "slight", "style", "pointed", "saved", "windows", "crossed", "louis", "pounds", "evidently", "principle", "immediate", "willing", "consequence", "richard", "principles",
"characters", "paul", "season", "remarked", "science", "tender", "worked", "grown", "whispered", "interested", "quarter", "midst", "liked", "advanced", "apparently", "bore", "active", "noticed", "aware", "thomas",
"uncle", "list", "dangerous", "august", "calm", "genius", "sacred", "kingdom", "entire", "popular", "unknown", "nice", "habit", "spanish", "familiar", "reader", "published", "direct", "handsome", "joined",
"actually", "kings", "posted", "approach", "washington", "hearing", "needed", "increased", "walking", "twice", "throw", "intellectual", "appointed", "wisdom", "ceased", "truly", "numbers", "demanded", "priest", "wounded",
"sorrow", "drive", "fault", "listened", "palace", "affair", "contact", "distinguished", "station", "beat", "distributed", "listen", "italy", "fool", "becomes", "watching", "hurt", "wants", "express", "occurred",
"favour", "height", "size", "edge", "subjects", "task", "follows", "interests", "nine", "sympathy", "burst", "putting", "dressed", "lifted", "hopes", "suffer", "noise", "smiling", "rode", "tells",
"minds", "farther", "literature", "vessel", "affection", "suffering", "proceeded", "flesh", "advice", "grand", "carrying", "legs", "spain", "post", "collection", "empty", "rank", "storm", "imagine", "wore",
"duties", "admitted", "countries", "pocket", "arrival", "imagination", "driven", "loud", "sentence", "lovely", "extraordinary", "november", "december", "happen", "absence", "breakfast", "population", "thank", "rules", "inhabitants",
"series", "laughing", "address", "relief", "bird", "owner", "impression", "satisfaction", "coat", "prepare", "relations", "shape", "birth", "rapidly", "smoke", "machine", "content", "consideration", "accompanied", "regular",
"moving", "stands", "wholly", "teeth", "busy", "treated", "burning", "shame", "quality", "bay", "discover", "inside", "brain", "soil", "completely", "message", "ring", "resolved", "calling", "phrase",
"acts", "mention", "square", "pair", "won", "title", "understanding", "fruit", "mad", "forces", "included", "tea", "rocks", "nearer", "slaves", "falling", "absolutely", "slow", "bearing", "mercy",
"larger", "explain", "contain", "grief", "soldier", "countenance", "previous", "explanation", "welcome", "proposed", "prayer", "stars", "germany", "belief", "informed", "moments", "poetry", "constant", "buy", "final",
"faithful", "ride", "policy", "supper", "drawing", "excitement", "dying", "demand", "fighting", "fields", "drove", "upper", "sum", "philip", "motion", "assistance", "forty", "april", "stones", "edward",
"fees", "kindly", "dignity", "catch", "october", "seated", "knees", "amongst", "current", "sending", "parties", "objects", "gained", "bitter", "possibly", "slave", "separate", "loose", "text", "receiving",
"worst", "sold", "don", "credit", "chosen", "hoped", "printed", "terror", "features", "fond", "control", "capable", "fifteen", "firm", "superior", "cruel", "spiritual", "harry", "splendid", "proof",
"pressed", "sooner", "join", "process", "crime", "dust", "instantly", "lands", "relation", "doors", "concerned", "deeply", "practical", "colour", "sing", "destroy", "anger", "distributing", "results", "increase",
"reasons", "nose", "friendly", "entrance", "rooms", "admit", "supply", "clean", "useful", "yesterday", "delicate", "fail", "continue", "remove", "addressed", "choice", "huge", "needs", "wear", "blind",
"unable", "cover", "double", "victory", "dozen", "constantly", "level", "india", "release", "rough", "ended", "shows", "fly", "praise", "devil", "ahead", "smith", "connected", "degrees", "gain",
"addition", "committed", "chamber", "notes", "italian", "gradually", "acquaintance", "bought", "souls", "mission", "sacrifice", "cities", "mistake", "exercise", "conscience", "based", "car", "buried", "theory", "commanded",
"nobody", "minister", "closely", "energy", "bare", "fought", "partly", "mistress", "hate", "arose", "playing", "color", "lake", "safety", "provisions", "description", "asleep", "centre", "faint", "thinks",
"parents", "escaped", "careful", "enjoy", "drop", "brilliant", "brief", "bringing", "worship", "goods", "tale", "skin", "roof", "grey", "highly", "crown", "castle", "excited", "throne", "stated",
"despair", "ease", "attached", "total", "kindness", "mile", "citizens", "circle", "dull", "extreme", "clouds", "figures", "intention", "prison", "term", "assured", "hidden", "thoroughly", "cup", "member",
"civil", "apply", "labor", "everywhere", "intelligence", "strike", "fairly", "comply", "fellows", "event", "gently", "connection", "protection", "conscious", "edition", "directed", "pulled", "flight", "evident", "surrounded",
"wishes", "yards", "voices", "weary", "couple", "variety", "whilst", "volume", "details", "older", "requirements", "custom", "apart", "bow", "awful", "everybody", "labour", "asking", "lover", "showing",
"introduced", "suit", "becoming", "composed", "plans", "rendered", "pictures", "lest", "volunteers", "singing", "eager", "precious", "paused", "require", "meat", "whenever", "milk", "dogs", "successful", "plants",
"vision", "rare", "granted", "raise", "egypt", "manners", "cousin", "development", "arthur", "cool", "trial", "learning", "approached", "bridge", "abroad", "devoted", "paying", "literary", "writer", "israel",
"disappeared", "interrupted", "stock", "readers", "dreadful", "female", "protect", "accustomed", "virginia", "type", "recognized", "salt", "destroyed", "signs", "innocent", "temper", "plenty", "pope", "avoid", "hurried",
"represented", "favor", "mental", "attitude", "returning", "admiration", "brothers", "anxiety", "queen", "teach", "count", "curiosity", "solemn", "causes", "vessels", "compelled", "dance", "hotel", "wicked", "fled",
"kissed", "guns", "fill", "visible", "younger", "guide", "earnest", "actual", "companions", "prisoner", "miserable", "lad", "harm", "views", "irish", "utterly", "ends", "shop", "stairs", "pardon",
"beg", "seldom", "kinds", "record", "fat", "sand", "violent", "branches", "inquired", "september", "worn", "ireland", "flat", "departure", "delivered", "gift", "ruin", "skill", "cattle", "equipment",
"temple", "calls", "earlier", "license", "visited", "consent", "sufficiently", "natives", "wound", "laughter", "contained", "perceived", "scattered", "whence", "rushed", "chiefly", "bold", "anywhere", "witness", "foolish",
"helped", "kitchen", "sell", "anybody", "self", "extremely", "treatment", "throat", "dreams", "patient", "speed", "growth", "quantity", "latin", "immense", "conclusion", "computer", "affected", "severe", "excuse",
"triumph", "origin", "joseph", "slept", "eternal", "thine", "audience", "pages", "sounds", "swift", "limited", "wings", "stepped", "services", "library", "remaining", "containing", "base", "confusion", "win",
"maid", "charming", "editions", "attended", "softly", "reality", "performed", "glorious", "likewise", "site", "sail", "frightened", "acquainted", "unhappy", "feared", "article", "prisoners", "store", "adopted", "shalt",
"remark", "cook", "thousands", "pause", "inclined", "convinced", "band", "valuable", "hence", "desert", "effects", "kiss", "plant", "ice", "ball", "stick", "absolute", "readily", "behold", "fierce",
"argument", "observe", "blessed", "bosom", "rage", "striking", "discovery", "creatures", "shouted", "guilty", "related", "setting", "forgot", "punishment", "gun", "slightly", "articles", "police", "mysterious", "extended",
"confess", "shade", "murder", "emotion", "destruction", "wondered", "increasing", "hide", "expedition", "horror", "local", "expenses", "ignorant", "doctrine", "generous", "range", "host", "wet", "cloud", "mystery",
"waste", "changes", "possess", "consciousness", "february", "trembling", "disease", "formerly", "spend", "production", "source", "mankind", "universal", "deck", "sees", "habits", "estate", "aunt", "reign", "humble",
"compliance", "delay", "shining", "reported", "hers", "unfortunate", "midnight", "listening", "flower", "hero", "accomplished", "doth", "classes", "thanks", "banks", "philosophy", "belong", "finger", "comfortable", "market",
"cap", "waves", "glanced", "troubled", "difficulties", "picked", "european", "purposes", "somewhere", "delighted", "pushed", "press", "household", "fleet", "baby", "region", "lately", "uttered", "exact", "image",
"ages", "murmured", "melancholy", "suspicion", "bowed", "refuse", "elizabeth", "staff", "liability", "enjoyed", "stretched", "gaze", "belonged", "ashamed", "reward", "meal", "blame", "nodded", "status", "opinions",
"indicate", "poem", "savage", "arise", "voyage", "misery", "guests", "painted", "attend", "afford", "donate", "job", "proceed", "loves", "forehead", "regret", "plainly", "risk", "lighted", "angel",
"rapid", "distinct", "doubtless", "properly", "wit", "fame", "singular", "error", "utmost", "methods", "reputation", "appeal", "strongly", "margaret", "lack", "breaking", "dawn", "violence", "fatal", "render",
"career", "design", "displayed", "gets", "commercial", "forgive", "lights", "agreeable", "suggestion", "utter", "sheep", "resolution", "spare", "patience", "domestic", "concluded", "farm", "reference", "chinese", "exist",
"corn", "approaching", "alike", "mounted", "jane", "issue", "key", "providing", "majority", "measures", "towns", "flame", "boston", "dared", "ignorance", "reduced", "occasionally", "weakness", "furnished", "china",
"priests", "flying", "cloth", "gazed", "profit", "fourth", "bell", "hitherto", "benefit", "movements", "eagerly", "acted", "urged", "ascii", "disposed", "electronically", "atmosphere", "chapter", "begged", "helen",
"hole", "invited", "borne", "departed", "catholic", "files", "reasonable", "sugar", "replacement", "sigh", "humanity", "thrust", "frame", "opposition", "disk", "haste", "lonely", "artist", "knight", "quarters",
"charm", "substance", "rolled", "email", "flung", "celebrated", "division", "slavery", "verse", "decision", "probable", "painful", "governor", "forever", "turns", "branch", "ocean", "rear", "leader", "delightful",
"stared", "boats", "keen", "disposition", "senses", "occasions", "readable", "beloved", "inches", "bones", "enthusiasm", "materials", "luck", "derived", "managed", "community", "apparent", "preserved", "magnificent", "hurry",
"scheme", "oil", "thence", "reaching", "dim", "wretched", "hanging", "pipe", "useless", "nevertheless", "print", "smooth", "solid", "pursued", "necessarily", "build", "attempted", "centuries", "eggs", "equivalent",
"hastily", "burned", "recent", "travel", "cries", "noon", "crying", "generations", "located", "cabin", "announcement", "britain", "compared", "handed", "cease", "smaller", "circumstance", "tent", "frequent", "alarm",
"nervous", "beast", "aloud", "independent", "gates", "distinction", "essential", "observation", "stronger", "recovered", "belonging", "loving", "masters", "writers", "permanent", "mortal", "stern", "gratitude", "preserve", "burden",
"aspect", "millions", "merry", "knife", "dread", "clever", "applicable", "district", "shadows", "jim", "silk", "failure", "links", "cent", "sentiment", "amid", "profits", "agent", "finds", "bade",
"desperate", "union", "imagined", "contempt", "raising", "lords", "hell", "separated", "grant", "seriously", "tribes", "hit", "enormous", "defective", "conviction", "secured", "mixed", "insisted", "wooden", "prefer",
"prayers", "fever", "selected", "daughters", "treat", "warning", "flew", "speaks", "developed", "impulse", "slipped", "ours", "mistaken", "damages", "ambition", "resumed", "christmas", "yield", "ideal", "schools",
"confirmed", "descended", "rush", "falls", "deny", "calculated", "correct", "perform", "somehow", "accordingly", "stayed", "acquired", "counsel", "distress", "sins", "notion", "discussion", "constitution", "anne", "hundreds",
"instrument", "firmly", "actions", "steady", "remarks", "empire", "elements", "idle", "pen", "entering", "online", "africa", "permit", "tide", "vol", "leaned", "college", "maintain", "sovereign", "tail",
"generation", "crowded", "fears", "nights", "limitation", "tied", "horrible", "cat", "displaying", "port", "male", "experienced", "opposed", "treaty", "contents", "rested", "mode", "poured", "les", "occur",
"seeking", "practically", "abandoned", "reports", "eleven", "sank", "begins", "founded", "brings", "trace", "instinct", "collected", "scotland", "characteristic", "chose", "cheerful", "tribe", "costs", "threatened", "arrangement",
"western", "sang", "beings", "sam", "pressure", "politics", "sorts", "shelter", "rude", "scientific", "revealed", "winds", "riding", "scenes", "shake", "industry", "claims", "merit", "profession", "lamp",
"interview", "territory", "sleeping", "sex", "coffee", "devotion", "thereof", "creation", "trail", "romans", "supported", "requires", "fathers", "prospect", "obey", "alexander", "shone", "operation", "northern", "nurse",
"profound", "hungry", "scott", "sisters", "assure", "exceedingly", "match", "wrath", "continually", "gifts", "folly", "chain", "uniform", "debt", "teaching", "venture", "execution", "shoes", "mood", "crew",
"perceive", "accounts", "eating", "multitude", "declare", "yard", "astonishment", "version", "vague", "odd", "grateful", "nearest", "infinite", "elsewhere", "copying", "apartment", "activity", "wives", "parted", "security",
"cared", "sensible", "owing", "martin", "saturday", "cottage", "jews", "leaning", "capacity", "joe", "settle", "referred", "francis", "holder", "involved", "sunshine", "dutch", "council", "princes", "ate",
"examination", "steel", "strangers", "beheld", "test", "noted", "slightest", "widow", "charity", "realized", "element", "shed", "errors", "communication", "reflection", "attacked", "organization", "maintained", "restored", "folks",
"concealed", "accordance", "heavens", "star", "examined", "deeds", "wordforms", "somebody", "incident", "oath", "guest", "bar", "row", "poverty", "bottle", "prevented", "bless", "stir", "intense", "completed",
"quarrel", "touching", "inner", "available", "fix", "resistance", "unusual", "deed", "derive", "hollow", "suspected", "contains", "sighed", "province", "deserted", "establishment", "vote", "muttered", "thither", "oxford",
"cavalry", "lofty", "endure", "succeed", "leg", "bid", "alice", "hated", "civilization", "acting", "landed", "christians", "passions", "interior", "scarce", "lightly", "disturbed", "supreme", "hang", "notwithstanding",
"shock", "exception", "offering", "display", "strain", "drank", "confined", "exhausted", "poets", "sounded", "aim", "critical", "jerusalem", "directions", "negro", "fearful", "standard", "studied", "bag", "buildings",
"consequences", "commenced", "deeper", "repeat", "driving", "beasts", "track", "rid", "holds", "residence", "steadily", "intimate", "drinking", "swear", "treasure", "fun", "throwing", "apt", "enterprise", "queer",
"seed", "tower", "runs", "defend", "favourite", "desires", "heavily", "assembled", "existed", "depends", "poems", "hesitated", "stuff", "section", "settlement", "staring", "sole", "roads", "plate", "mexico",
"overcome", "pains", "performing", "dwell", "grounds", "taxes", "marble", "recently", "tones", "ability", "awake", "walter", "wave", "shaking", "folk", "possibility", "butter", "fury", "marched", "moses",
"writes", "issued", "sailed", "instructions", "hatred", "pursuit", "pull", "furniture", "additions", "hid", "rope", "adventure", "royalty", "vanished", "arts", "elder", "signal", "wanting", "supplied", "feast",
"safely", "burn", "describe", "references", "lesson", "annual", "card", "passes", "application", "intelligent", "county", "beaten", "presents", "format", "flow", "sixty", "scale", "damage", "marks", "obtaining",
"moreover", "commerce", "startled", "southern", "consequently", "outer", "belongs", "ben", "wrought", "average", "naked", "conducted", "rivers", "songs", "obvious", "foundation", "concern", "ceremony", "magic", "campaign",
"hunting", "carolina", "liberal", "whisper", "largely", "commonly", "torn", "exists", "contributions", "hunt", "teacher", "christianity", "lawyer", "operations", "detail", "shortly", "caesar", "wondering", "leaders", "blessing",
"princess", "altar", "tenderness", "tiny", "web", "cardinal", "sharply", "regiment", "chest", "distinctly", "purple", "creating", "gather", "depth", "indignation", "performance", "election", "prosperity", "gloomy", "conception",
"clerk", "decide", "drunk", "victim", "reflected", "pour", "preceding", "individuals", "gazing", "absurd", "lift", "gesture", "armies", "limbs", "manage", "brethren", "hugh", "plays", "hastened", "dragged",
"motive", "whatsoever", "pointing", "verses", "pronounced", "exchange", "definite", "emperor", "tendency", "remote", "finish", "flag", "boots", "enabled", "administration", "denied", "churches", "rarely", "earnestly", "considering",
"previously", "ugly", "bears", "signed", "genuine", "harmless", "mingled", "obedience", "walks", "training", "badly", "feed", "central", "contrast", "relieved", "romance", "structure", "payment", "pace", "passages",
"succession", "persuaded", "sources", "inquiry", "inspired", "angels", "roll", "wilt", "inch", "troubles", "perfection", "wherever", "owe", "handle", "advantages", "trip", "shoot", "fortunate", "newspaper", "employment",
"fitted", "refuge", "misfortune", "providence", "owns", "cutting", "beard", "stirred", "tear", "resist", "depths", "maiden", "determine", "commission", "merchant", "whereas", "crossing", "independence", "lively", "breeze",
"provinces", "virtues", "conceived", "relative", "solitary", "smell", "wandering", "thereby", "eighteen", "locked", "provision", "courts", "eaten", "historical", "regarding", "preferred", "pick", "ruined", "wherein", "vanity",
"condemned", "deliver", "unexpected", "desk", "gross", "lane", "happens", "represent", "root", "mud", "respectable", "cleared", "feels", "fruits", "testimony", "existing", "bride", "rang", "ranks", "responsibility",
"beating", "disappointed", "suitable", "depend", "judges", "giant", "grasp", "arrive", "simplicity", "autumn", "absent", "legally", "veil", "gloom", "doubtful", "suspect", "weapons", "limits", "determination", "feeble",
"prophet", "gathering", "basis", "examine", "corrupt", "payments", "returns", "laying", "prize", "instances", "theatre", "purchase", "comparison", "composition", "rival", "someone", "realize", "defeat", "demands", "foe",
"shared", "consists", "studies", "balance", "intercourse", "forming", "slender", "coach", "criminal", "knocked", "silly", "humour", "masses", "indifferent", "recall", "occupation", "discourse", "keeps", "regions", "intervals",
"assist", "novel", "intellect", "leads", "hither", "tales", "sale", "revenge", "yonder", "resources", "jealous", "wheel", "invitation", "narrative", "risen", "burnt", "sentiments", "inferior", "amusement", "flash",
"recognize", "swiftly", "portrait", "create", "summoned", "suggest", "induced", "conflict", "fed", "curse", "disappointment", "helpless", "preparing", "construction", "zeal", "responsible", "indicated", "groups", "positive", "attracted",
"vengeance", "fort", "club", "cure", "stout", "missed", "gracious", "include", "flood", "satisfy", "agony", "respects", "ventured", "implied", "stupid", "seas", "grain", "enjoyment", "wearing", "indifference",
"conceal", "horizon", "pleasures", "therein", "precisely", "assume", "registered", "estimate", "steep", "route", "gardens", "visitor", "closer", "harmony", "non", "thunder", "wire", "graceful", "crept", "childhood",
"knee", "saddle", "supplies", "weeping", "mostly", "paragraphs", "unconscious", "mutual", "scorn", "grows", "external", "agents", "software", "institutions", "losing", "universe", "clock", "attempts", "instruction", "injury",
"roots", "receipt", "jumped", "dearest", "sore", "earliest", "finest", "enable", "discipline", "motives", "fastened", "introduction", "converted", "wilderness", "confused", "fancied", "offices", "slip", "revolution", "wedding",
"farmer", "silently", "fires", "wept", "behalf", "reckon", "responded", "uncertain", "neglected", "stroke", "exquisite", "engagement", "dirty", "rolling", "platform", "messenger", "privilege", "admirable", "offers", "mischief",
"physician", "imposed", "organized", "covering", "student", "daring", "cave", "wars", "convey", "sincere", "tradition", "gravely", "combined", "gallant", "sensation", "travelling", "charges", "submit", "tragedy", "specific",
"commander", "inn", "stiff", "accompany", "score", "virgin", "farewell", "paradise", "villages", "hunger", "trembled", "favorite", "criticism", "proprietary", "customs", "cotton", "hospital", "restrictions", "outward", "impressed",
"blows", "plains", "flashed", "rent", "prey", "owed", "longing", "musical", "satisfactory", "ridiculous", "sheet", "disgrace", "colored", "originally", "wages", "papa", "gas", "inevitable", "extensive", "leisure",
"deadly", "chin", "claimed", "glow", "emotions", "jealousy", "leaf", "publication", "hostile", "wandered", "railway", "translation", "procession", "betrayed", "pound", "admired", "elected", "sunk", "ruins", "eastern",
"roses", "citizen", "reminded", "deceived", "tables", "beach", "starting", "funeral", "arrested", "flour", "feature", "correspondence", "consisted", "counted", "reserve", "proceedings", "roar", "romantic", "hut", "strangely",
"absorbed", "propose", "seats", "bark", "reception", "pleasing", "attained", "wake", "research", "prayed", "monarch", "clothing", "dollar", "illness", "calmly", "obeyed", "heartily", "pressing", "daylight", "warriors",
"jest", "abruptly", "washed", "comment", "metal", "preparations", "nerves", "solution", "pretended", "sixteen", "assembly", "tobacco", "entity", "dwelling", "depart", "swung", "bitterly", "alteration", "colony", "disclaimers",
"wing", "peaceful", "lion", "opportunities", "alarmed", "furnish", "resting", "accused", "culture", "writings", "dwelt", "conquered", "trick", "trusted", "column", "financial", "cunning", "preparation", "drama", "joke",
"entertained", "mist", "hypertext", "shell", "medicine", "proofread", "nest", "reverence", "situated", "yielded", "conceive", "appointment", "lessons", "fetch", "tomb", "candle", "offence", "coarse", "heap", "mixture",
"homes", "model", "defect", "destined", "occasional", "fourteen", "hint", "knights", "solicit", "dreamed", "objection", "craft", "acid", "namely", "neglect", "data", "weapon", "confessed", "arrangements", "repose",
"complying", "copied", "pink", "user", "heels", "grandfather", "income", "regards", "streams", "vigorous", "accepting", "bishop", "lightning", "authors", "flames", "observations", "compressed", "sport", "powder", "beds",
"orange", "painting", "shout", "bath", "careless", "chap", "derivative", "roused", "primitive", "doorway", "climbed", "volumes", "vulgar", "arguments", "sunset", "convenient", "mail", "recalled", "wrapped", "abode",
"planted", "paint", "surrender", "establish", "mild", "promptly", "appearing", "department", "parish", "nay", "lit", "handkerchief", "basket", "easier", "deserve", "quit", "assurance", "mirror", "plot", "yer",
"upward", "sadly", "secretary", "adding", "modest", "dish", "cares", "straw", "net", "advised", "heavenly", "largest", "proceeding", "impatient", "wounds", "warmth", "certainty", "restless", "meantime", "rays",
"salvation", "lovers", "experiment", "shores", "today", "tremendous", "afforded", "moonlight", "intend", "cultivated", "flushed", "newspapers", "rocky", "pious", "wont", "steam", "improvement", "garments", "treasury", "merchants",
"perpetual", "trained", "products", "affectionate", "dispute", "visitors", "poison", "proposition", "maybe", "rifle", "warned", "parting", "shield", "erected", "employ", "prevailed", "talent", "rises", "climate", "chairs",
"searched", "unlike", "recover", "mate", "arrange", "fortunes", "puzzled", "committee", "aged", "ashes", "ghost", "promises", "bushes", "effective", "distinguish", "manifest", "comparatively", "esteem", "blew", "revelation",
"wash", "recognition", "confession", "clay", "nonsense", "trunk", "management", "undoubtedly", "dried", "chiefs", "coal", "stolen", "earthly", "restore", "indirectly", "lasted", "selfish", "renewed", "canoe", "protest",
"vice", "races", "deemed", "temporary", "pile", "chapel", "moderate", "spell", "upright", "quoted", "area", "bone", "solitude", "instruments", "formal", "students", "greatness", "struggling", "reproach", "altered",
"grim", "leaped", "federal", "questioned", "editor", "desirable", "acknowledge", "motionless", "remedy", "bestowed", "pursue", "representative", "pole", "gladly", "linen", "vital", "sink", "pacific", "hopeless", "dangers",
"gratefully", "president", "travelled", "ward", "nephew", "cheer", "bloody", "siege", "commands", "justified", "stomach", "improved", "admire", "openly", "sailors", "abide", "advancing", "forests", "records", "recorded",
"modification", "dramatic", "statements", "upstairs", "varied", "letting", "comrades", "sets", "descent", "whither", "envy", "load", "pretend", "folded", "brass", "internal", "furious", "curtain", "healthy", "obscure",
"summit", "alas", "fifth", "center", "faced", "cheap", "saints", "colonel", "contest", "owned", "adventures", "exclusion", "seize", "chances", "springs", "alter", "landing", "fence", "leagues", "glimpse",
"statue", "contract", "luxury", "artillery", "doubts", "saving", "fro", "string", "combination", "awakened", "faded", "arrest", "protected", "temperature", "strict", "contented", "professional", "intent", "injured", "neighborhood",
"abundance", "smoking", "yourselves", "medical", "garrison", "likes", "corps", "heroic", "inform", "retained", "agitation", "nobles", "prominent", "institution", "judged", "embrace", "wheels", "closing", "damaged", "pack",
"affections", "eldest", "anguish", "surrounding", "obviously", "strictly", "capture", "drops", "inquire", "ample", "remainder", "justly", "recollection", "deer", "answers", "bedroom", "purely", "bush", "plunged", "thyself",
"joint", "refer", "expecting", "madam", "railroad", "spake", "respecting", "columns", "weep", "identify", "discharge", "bench", "heir", "oak", "rescue", "limit", "unpleasant", "anxiously", "innocence", "awoke",
"expectation", "incomplete", "program", "reserved", "secretly", "invention", "faults", "disagreeable", "piano", "defeated", "charms", "purse", "persuade", "deprived", "electric", "endless", "interval", "chase", "heroes", "invisible",
"occupy", "gown", "cruelty", "lock", "lowest", "hesitation", "withdrew", "proposal", "destiny", "recognised", "commons", "foul", "loaded", "amidst", "titles", "ancestors", "types", "commanding", "madness", "happily",
"assigned", "declined", "temptation", "subsequent", "jewels", "breathed", "willingly", "youthful", "bells", "spectacle", "uneasy", "shine", "formidable", "stately", "machinery", "fragments", "rushing", "attractive", "product", "economic",
"sickness", "uses", "dashed", "engine", "ashore", "dates", "theirs", "adv", "clasped", "international", "leather", "spared", "crushed", "interfere", "subtle", "waved", "slope", "floating", "worry", "effected",
"passengers", "violently", "donation", "steamer", "witnesses", "specified", "learnt", "stores", "designed", "guessed", "roger", "timber", "talents", "heed", "jackson", "murdered", "vivid", "woe", "calculate", "killing",
"laura", "savages", "wasted", "trifle", "funny", "pockets", "philosopher", "insult", "den", "representation", "incapable", "eloquence", "dine", "temples", "ann", "sensitive", "robin", "appetite", "wishing", "picturesque",
"douglas", "courtesy", "flowing", "remembrance", "lawyers", "sphere", "murmur", "elegant", "honourable", "stopping", "guilt", "welfare", "avoided", "fishing", "perish", "sober", "steal", "delicious", "infant", "lip",
"norman", "offended", "dost", "memories", "wheat", "japanese", "humor", "exhibited", "encounter", "footsteps", "marquis", "smiles", "amiable", "twilight", "arrows", "consisting", "park", "retire", "economy", "sufferings",
"secrets", "halted", "govern", "favourable", "colors", "translated", "stretch", "formation", "immortal", "gallery", "parallel", "lean", "tempted", "frontier", "continent", "knock", "impatience", "unity", "dealing", "prohibition",
"decent", "fiery", "images", "tie", "punished", "submitted", "julia", "albert", "rejoined", "speedily", "consented", "major", "preliminary", "cell", "void", "placing", "prudence", "egg", "amazement", "border",
"artificial", "hereafter", "fanny", "crimes", "breathe", "exempt", "anchor", "chicago", "sits", "purchased", "eminent", "neighbors", "glowing", "sunlight", "examples", "exercised", "wealthy", "seeming", "bonaparte", "shouting",
"thanked", "illustrious", "curiously", "inspiration", "seeds", "naval", "foes", "everyone", "longed", "abundant", "doubted", "painter", "greeted", "erect", "glasses", "meanwhile", "shooting", "athens", "wagon", "lend",
"lent", "crisis", "undertake", "particulars", "veins", "polite", "anna", "experiences", "seal", "header", "clergy", "mount", "array", "corners", "magazine", "loudly", "bitterness", "texas", "guardian", "searching",
"rejected", "harsh", "includes", "boldly", "maurice", "kate", "lunch", "pine", "shells", "seconds", "despite", "hoping", "injustice", "expressions", "flies", "push", "tight", "problems", "landscape", "sue",
"protested", "scarlet", "abandon", "artistic", "mainly", "measured", "loyal", "boiling", "desirous", "suited", "alliance", "advise", "waist", "sinking", "apprehension", "stable", "gregory", "maximum", "commit", "hideous",
"hamilton", "sweetness", "dismissed", "tore", "affect", "shaken", "evils", "unworthy", "significance", "modified", "miracle", "lieu", "peasant", "considerably", "observing", "conveyed", "resemblance", "extend", "riches", "personally",
"morality", "rebellion", "thread", "dumb", "inclination", "forbidden", "copper", "differences", "sailor", "requested", "response", "promoting", "imperial", "blank", "purity", "victor", "bending", "solemnly", "minor", "del",
"crimson", "republic", "teachers", "danced", "bargain", "dealt", "fatigue", "telephone", "cents", "whip", "dislike", "witnessed", "infantry", "acres", "checked", "countrymen", "companies", "normal", "shirt", "addresses",
"introduce", "sofa", "mothers", "sweep", "conversion", "sketch", "deserved", "answering", "virtuous", "anyway", "thief", "driver", "retain", "constructed", "conspicuous", "channel", "nobility", "editing", "declaration", "guards",
"personality", "smallest", "excess", "separation", "disgust", "accomplish", "speeches", "convent", "rightly", "suspended", "reform", "mob", "thirst", "unnecessary", "treasures", "asks", "viewed", "designs", "gleam", "threatening",
"palm", "filling", "quoth", "fur", "fortnight", "holes", "addressing", "frightful", "encourage", "speaker", "tribute", "procure", "frankly", "recommended", "relieve", "intentions", "unjust", "legislation", "project", "threshold",
"merits", "morrow", "traces", "induce", "spear", "inward", "pupils", "corresponding", "fairy", "conclude", "clung", "neat", "lucky", "lap", "session", "torture", "damp", "ridge", "spoil", "liable",
"swords", "hearty", "thoughtful", "traveller", "chains", "favorable", "tin", "strongest", "dependent", "couch", "bills", "warrant", "complaint", "endeavour", "sails", "dined", "convention", "guarded", "angle", "widely",
"endeavoured", "ardent", "cow", "mill", "victims", "prejudice", "foremost", "map", "probability", "porch", "lieutenant", "surprising", "fountain", "sustained", "appropriate", "assisted", "rejoice", "extending", "marvellous", "clothed",
"collar", "bands", "confident", "hasty", "nigh", "organ", "prose", "privileges", "selection", "inquiries", "codes", "replace", "saint", "districts", "deliberately", "awe", "beforehand", "strife", "released", "compare",
"beer", "retorted", "relate", "cheerfully", "pistol", "presume", "velvet", "wretch", "stirring", "righteousness", "missing", "fain", "facing", "fashionable", "producing", "peoples", "positively", "reasoning", "gravity", "disturb",
"sermon", "exchanged", "partner", "brains", "lowered", "association", "estates", "abuse", "flock", "niece", "languages", "asserted", "bodily", "notions", "faculty", "cannon", "thirteen", "sailing", "rings", "smart",
"possessions", "disciples", "petty", "widest", "divisions", "prudent", "caution", "justify", "awhile", "boxes", "manuscript", "cigar", "warrior", "impressions", "aught", "lifting", "inaccurate", "tidings", "liquid", "staying",
"concept", "creek", "brush", "download", "specially", "cream", "meetings", "jump", "unwilling", "adapted", "practised", "combat", "subdued", "innumerable", "blowing", "extra", "civilized", "invented", "pitch", "cliff",
"crowned", "portions", "awkward", "horrid", "pulling", "appreciate", "communicated", "jury", "encountered", "attacks", "monster", "maintaining", "sites", "frozen", "invariably", "dies", "survive", "literally", "consolation", "phenomena",
"pot", "briefly", "rice", "planned", "respected", "sublime", "dropping", "guy", "behaviour", "desolate", "penny", "adopt", "replaced", "revenue", "formats", "hired", "regularly", "infringement", "curtains", "eagerness",
"helping", "investigation", "constitutional", "insist", "occurs", "fools", "inheritance", "latest", "leap", "games", "apple", "visiting", "travellers", "experiments", "pupil", "enjoying", "totally", "twisted", "discuss", "firing",
"background", "subscribe", "tenderly", "transcribe", "descend", "differ", "avail", "disaster", "bet", "periodic", "bull", "entertainment", "computers", "cursed", "raw", "fulfilled", "virus", "log", "skies", "scotch",
"embraced", "hospitality", "faintly", "robbed", "cart", "influences", "ascended", "incidents", "childish", "robe", "aboard", "resembling", "reflect", "dominion", "dreary", "serving", "complexion", "engage", "tents", "herd",
"attain", "collect", "disclaims", "pan", "relatives", "borrowed", "convert", "outline", "blown", "comprehend", "peasants", "opera", "assault", "deceive", "doctrines", "representatives", "dedicated", "struggled", "officials", "hiding",
"paths", "backs", "prominently", "prices", "procured", "mourning", "compliment", "heights", "approval", "gasped", "breadth", "withdraw", "tune", "compassion", "polished", "latitude", "dishes", "parent", "contrived", "delicacy",
"projected", "akin", "betray", "traced", "resentment", "indemnify", "pseud", "sacrifices", "disguise", "transcription", "document", "neighbour", "squire", "punish", "bars", "glittering", "tossed", "block", "lots", "worldly",
"muscles", "elbow", "obligation", "trifling", "decline", "attachment", "ambitious", "filename", "artists", "bloom", "holiday", "brute", "repair", "fist", "recollect", "eagle", "honorable", "significant", "barren", "functions",
"guided", "dense", "fiction", "adds", "rows", "recommend", "suspicious", "resulting", "seventy", "shillings", "educational", "duly", "governed", "scripture", "upwards", "sworn", "horn", "brook", "fund", "lodge",
"infinitely", "clergyman", "marshal", "ruled", "fiercely", "costume", "pit", "disorder", "sheer", "exalted", "fare", "applause", "remind", "binary", "packed", "pillow", "jersey", "abbey", "nowhere", "anyhow",
"agitated", "marching", "catching", "grasped", "arrow", "tend", "carved", "fitting", "bonds", "instructed", "elaborate", "corpse", "bewildered", "essence", "positions", "edited", "continues", "elevation", "realm", "debts",
"glancing", "shops", "complained", "loyalty", "coin", "clad", "staircase", "documents", "interpreted", "extremity", "accord", "sally", "lace", "tremble", "exile", "gospel", "mechanical", "successfully", "scholar", "wonders",
"temperament", "expressing", "trap", "spots", "awaiting", "potatoes", "likeness", "harbour", "proofs", "jolly", "contributed", "wits", "generosity", "ruler", "cake", "lamps", "crazy", "sincerity", "entertain", "madame",
"sir", "faculties", "hesitate", "deepest", "seventeen", "lordship", "greeting", "feminine", "monstrous", "tongues", "barely", "mansion", "facility", "praised", "warranties", "happier", "indicating", "rob", "gigantic", "honey",
"ladder", "ending", "swallowed", "sunny", "knelt", "tyranny", "decree", "stake", "divide", "dreaming", "proclaimed", "dignified", "tread", "mines", "viewing", "defense", "oldest", "incredible", "bidding", "brick",
"arch", "everlasting", "elect", "sprung", "harder", "winding", "deductible", "magistrate", "respective", "liquor", "imitation", "shy", "perished", "prime", "studying", "eighty", "unfortunately", "licensed", "fog", "coloured",
"bits", "consult", "moves", "warn", "vile", "depended", "legend", "locations", "shallow", "doom", "dreaded", "encouragement", "impatiently", "scent", "varieties", "irregular", "battles", "compass", "neighbouring", "bliss",
"harvest", "promotion", "stove", "faithfully", "excellence", "transfer", "awaited", "heathen", "poetic", "consulted", "illustrated", "fundamental", "bundle", "rebel", "cultivation", "joys", "rigid", "tragic", "review", "representing",
"flowed", "brows", "whereupon", "terribly", "melted", "venerable", "towers", "cooking", "suspicions", "oppressed", "revolt", "swell", "improve", "describes", "goddess", "wreck", "convince", "sentences", "bowl", "radiant",
"westward", "indignant", "refined", "unseen", "illustration", "pertaining", "swamp", "congregation", "nerve", "undertaking", "disclaimer", "characteristics", "stare", "specimens", "ascertain", "pledge", "earn", "warfare", "supposing", "subsequently",
"attending", "angrily", "select", "animated", "industrial", "hurriedly", "manhood", "quantities", "interpretation", "dressing", "rejoiced", "challenge", "produces", "forbid", "gang", "boiled", "shouts", "theme", "thankful", "admission",
"enters", "elevated", "pool", "terrified", "lads", "persisted", "conference", "equality", "genus", "didst", "newly", "generals", "surroundings", "sorrows", "occasioned", "invasion", "workmen", "monks", "sends", "discretion",
"pattern", "reveal", "endured", "resolve", "preach", "exceeding", "ringing", "triumphant", "defiance", "errand", "woke", "grandmother", "weighed", "wool", "communicate", "strikes", "promising", "scenery", "righteous", "essentially",
"oppose", "joyous", "specimen", "doctors", "eloquent", "manager", "organs", "sticks", "drag", "haunted", "chorus", "rational", "crop", "processing", "accurate", "wolf", "adorned", "sheets", "resort", "refusal",
"bond", "vicinity", "preacher", "sympathetic", "casting", "opens", "prophets", "horns", "warmly", "salary", "continuous", "continual", "defended", "breaks", "workers", "lantern", "balls", "rod", "blaze", "examining",
"peculiarly", "vegetables", "ingenious", "excite", "horseback", "farmers", "wildly", "mouths", "carpet", "sadness", "customary", "circles", "wonderfully", "max", "successor", "allied", "ceiling", "confirmation", "glances", "diamonds",
"goal", "representations", "cash", "vacant", "antiquity", "despise", "lawn", "appealed", "turkey", "texts", "neighbor", "spreading", "discharged", "phrases", "ultimate", "tastes", "submission", "entry", "blush", "monument",
"hardy", "thorough", "ein", "ecclesiastical", "fertile", "exciting", "captive", "severity", "considerations", "shew", "faster", "grandeur", "winning", "solely", "globe", "malice", "echoed", "lodging", "conservative", "throng",
"prosperous", "whistle", "floated", "transferred", "declaring", "reckoned", "cheese", "bite", "thoughtfully", "breach", "enthusiastic", "cars", "downstairs", "allowing", "invite", "adjoining", "dusk", "cathedral", "truths", "plague",
"sandy", "boil", "beautifully", "inhabited", "tomorrow", "exclamation", "finishing", "shocked", "escort", "forgetting", "hanged", "hats", "mirth", "uncomfortable", "bows", "pierced", "harbor", "tricks", "rubbed", "apparatus",
"mysteries", "honesty", "negroes", "concerns", "wander", "assert", "ceremonies", "sacrificed", "utterance", "dismay", "fright", "rail", "reflections", "crops", "pushing", "proves", "jimmy", "pathetic", "imperfect", "haughty",
"navy", "fortress", "hurrying", "blessings", "attempting", "insects", "selling", "appreciation", "suppressed", "acquire", "offensive", "ripe", "dresses", "reigned", "coldly", "candles", "sixth", "blazing", "youngest", "mask",
"lecture", "parlor", "decidedly", "whereby", "reverend", "successive", "perception", "buffalo", "sire", "quitted", "keys", "develop", "function", "morals", "damned", "vexed", "pouring", "bullet", "excessive", "bind",
"identical", "cliffs", "tools", "piety", "superstition", "git", "substantial", "bulk", "prevail", "wiser", "preaching", "prolonged", "annoyed", "splendour", "remembering", "upset", "cab", "bunch", "pencil", "subjected",
"vegetable", "exhibit", "emerged", "cooked", "hay", "gale", "preached", "trousers", "debate", "dated", "tumult", "corruption", "summons", "comrade", "eternity", "hears", "lingered", "propriety", "stillness", "partial",
"welcomed", "cabinet", "proceeds", "vow", "quaint", "soup", "beef", "rests", "slay", "surgeon", "irresistible", "sealed", "repeating", "allowance", "undertaken", "treachery", "posts", "borders", "attendant", "unite",
"murderer", "owners", "sweeping", "unconsciously", "blade", "saviour", "theories", "behaved", "pleaded", "spy", "possesses", "lawful", "seasons", "withdrawn", "reckless", "factory", "shades", "gossip", "seventh", "attendance",
"robes", "journal", "systems", "token", "intimacy", "abstract", "machines", "bestow", "chanced", "locks", "honestly", "legitimate", "accent", "symptoms", "votes", "ragged", "manifested", "fidelity", "swinging", "descending",
"sincerely", "bred", "whereof", "indies", "novels", "league", "failing", "succeeding", "approve", "cautiously", "miller", "afflicted", "lodgings", "petition", "traffic", "sparkling", "limb", "architecture", "disposal", "carriages",
"crack", "kindred", "naught", "ornament", "slew", "steward", "fantastic", "evolution", "patiently", "reverse", "survey", "dug", "amuse", "stretching", "forthwith", "contemporary", "foliage", "receives", "scandal", "donors",
"deliberate", "influenced", "intolerable", "hearth", "symbol", "governments", "repaired", "pleasantly", "homage", "victorious", "recovery", "defined", "attendants", "modesty", "washing", "pavement", "unnatural", "decisive", "wisely", "precise",
"negative", "occurrence", "snatched", "shaft", "linked", "festival", "exclusively", "wickedness", "visions", "rosy", "carelessly", "stem", "corporation", "feeding", "cows", "schemes", "preference", "urge", "husbands", "labours",
"shrill", "exercises", "sovereignty", "reduce", "distressed", "clearing", "removal", "dean", "assertion", "accessible", "comedy", "flush", "code", "philosophers", "adequate", "vaguely", "treason", "hunter", "chambers", "split",
"yielding", "newsletter", "snake", "historian", "ass", "intensity", "democracy", "battery", "draws", "creed", "liking", "tyrant", "strove", "attraction", "slaughter", "dismal", "deposited", "assent", "cups", "concert",
"downward", "canal", "evenings", "wax", "detective", "fancies", "spoiled", "revolver", "earned", "analysis", "finer", "paces", "roaring", "prompt", "paperwork", "wherefore", "emphasis", "sharing", "delayed", "inherited",
"bronze", "waking", "garment", "redistributing", "wholesome", "remorse", "stooped", "dew", "monk", "thrill", "hue", "exclusive", "funds", "porter", "uncommon", "dash", "strained", "confounded", "swim", "strip",
"ultimately", "team", "missionary", "esteemed", "tracks", "envelope", "whoever", "expensive", "headquarters", "cherished", "brandy", "startling", "homer", "talks", "acute", "cigarette", "motor", "embarrassed", "volunteer", "offspring",
"network", "reaches", "indispensable", "plane", "reaction", "regiments", "sums", "partially", "prejudices", "proudly", "baggage", "terrace", "deaf", "allusion", "grip", "juice", "resigned", "humility", "blast", "ministry",
"sexual", "diameter", "troop", "onward", "crowds", "marrying", "tightly", "sullen", "brutal", "axe", "penalty", "tops", "diamond", "boards", "corridor", "endowed", "strengthened", "cells", "proportions", "alternate",
"echo", "restraint", "trials", "reads", "identity", "headed", "softened", "quivering", "stages", "sway", "poetical", "objected", "screen", "professed", "dirt", "ascertained", "era", "wider", "ambassador", "constituted",
"breed", "interference", "eyebrows", "shapes", "afar", "consist", "acceptance", "displays", "flashing", "hunted", "beauties", "lazy", "shrewd", "extravagant", "momentary", "cordial", "engineer", "rapidity", "halt", "alternative",
"devils", "stamp", "compact", "whites", "breathless", "encoding", "drift", "disappear", "roared", "revived", "counter", "imaginary", "diminished", "honoured", "despatched", "objections", "ray", "climbing", "attract", "astonishing",
"competition", "suggestions", "ink", "oft", "crystal", "shower", "diseases", "obedient", "draught", "wondrous", "await", "armour", "massive", "bottles", "kin", "cellar", "falsehood", "pillars", "philosophical", "worlds",
"memorable", "detected", "stealing", "noisy", "henceforth", "laden", "frost", "device", "glare", "touches", "senate", "lasting", "communion", "transport", "coffin", "eventually", "enclosed", "forgiveness", "awfully", "clinging",
"darkened", "contemplation", "termed", "manufacture", "swallow", "commonplace", "resembled", "labors", "contracted", "inscription", "comfortably", "indulge", "indulgence", "bravely", "kneeling", "yea", "keenly", "exhibition", "agricultural", "enlightened",
"quest", "compliments", "crest", "extension", "uneasiness", "constitute", "inflicted", "lakes", "swing", "meadow", "noblest", "downloading", "complex", "controversy", "freed", "resignation", "tempest", "guidance", "prospects", "humbly",
"lined", "serene", "shrugged", "honours", "roughly", "checks", "remarkably", "dainty", "overhead", "commencement", "singularly", "brightness", "oppression", "repeatedly", "conspiracy", "restrain", "splendor", "preservation", "pub", "pepper",
"basin", "creeping", "publicly", "continuing", "grove", "calamity", "pony", "vigour", "melody", "profitable", "descendants", "hire", "speculation", "discoveries", "accepts", "drunken", "candidate", "principally", "worried", "obstinate",
"hasten", "foreigners", "elderly", "overwhelmed", "instincts", "telegraph", "university", "ghastly", "patron", "varying", "barbarous", "celestial", "patriotism", "modify", "earnestness", "exertion", "fox", "refusing", "horsemen", "inspection",
"stations", "grieved", "louder", "bursting", "regretted", "mournful", "pursuing", "traitor", "associations", "cautious", "foundations", "stamped", "prior", "undertook", "telegram", "beggar", "chimney", "complicated", "striving", "magistrates",
"converse", "graces", "wiped", "oars", "apology", "scared", "imprisonment", "eastward", "substitute", "handful", "usage", "lodged", "villain", "banished", "restoration", "serpent", "hedge", "jurisdiction", "captains", "settlers",
"gaining", "valiant", "primary", "storms", "beam", "tour", "prophecy", "spectacles", "obsolete", "buying", "shepherd", "wells", "exaggerated", "heated", "penetrated", "travels", "earl", "hereditary", "supernatural", "competent",
"piled", "hostess", "agriculture", "boughs", "urgent", "gratified", "suffice", "ports", "drifted", "accuracy", "deceased", "circular", "securing", "possibilities", "alert", "neighboring", "democratic", "bud", "accounted", "aided",
"blanket", "hail", "pretence", "beams", "pig", "shaped", "oven", "rounded", "ivory", "northward", "isolated", "policeman", "conventional", "dusty", "bishops", "complaints", "stripped", "plead", "hinder", "cord",
"flows", "personage", "classical", "alongside", "wrongs", "extract", "rewarded", "lungs", "lighter", "kisses", "serves", "pint", "forgiven", "sternly", "proclamation", "realised", "pipes", "arising", "pitched", "tube",
"observer", "smote", "avenue", "elephant", "footing", "statesman", "rebels", "nails", "wears", "doomed", "edges", "affecting", "stormy", "bee", "bury", "efficient", "mix", "supporting", "actor", "disturbance",
"sweat", "executive", "seemingly", "tenth", "blossoms", "folds", "painfully", "polish", "shudder", "roofs", "comparative", "begging", "imposing", "notable", "invested", "imprisoned", "mute", "cage", "esq", "cured",
"cargo", "negotiations", "assented", "jail", "skilful", "ideals", "conferred", "resulted", "illusion", "torment", "troublesome", "crowns", "repentance", "blankets", "proprietor", "uncertainty", "concentrated", "covers", "scream", "compromise",
"respectful", "chariot", "ammunition", "bonnet", "secondary", "persecution", "lesser", "assistant", "saluted", "fits", "indulged", "springing", "cane", "fold", "boundary", "valued", "rugged", "aloft", "thieves", "parlour",
"indebted", "tons", "processes", "argue", "dearly", "logic", "panic", "restrained", "vainly", "weariness", "enlarged", "tasted", "rural", "torrent", "resolute", "refrain", "kissing", "gorgeous", "meets", "circulation",
"passionately", "inasmuch", "unexpectedly", "stress", "consumption", "groan", "suits", "sustain", "hosts", "crash", "resemble", "epoch", "quote", "lacking", "nominally", "choked", "aristocracy", "granite", "gradual", "delights",
"hurled", "joyful", "sack", "slumber", "detached", "snapped", "shadowy", "accompanying", "annoyance", "crush", "needle", "repent", "phenomenon", "execute", "canst", "smoked", "greet", "monarchy", "behave", "richly",
"controlled", "strive", "endeavor", "barrier", "curve", "politeness", "flora", "rely", "flank", "convenience", "courteous", "logs", "lamb", "effectually", "logical", "dimly", "withered", "diet", "praises", "fulfil",
"mantle", "discussing", "chicken", "judicial", "consistent", "ridicule", "reins", "barrel", "distrust", "trunks", "verily", "hunters", "feather", "desperately", "goodly", "habitual", "voluntary", "luncheon", "eighteenth", "exertions",
"expert", "coolly", "mistakes", "tedious", "contemplated", "jacket", "gleaming", "shrank", "swimming", "perplexed", "impressive", "universally", "displeasure", "maids", "rates", "underneath", "expedient", "impress", "bees", "bounded",
"worshipped", "resisted", "provincial", "popularity", "baker", "shattered", "merciful", "olive", "tramp", "compensation", "martial", "genial", "conjecture", "van", "waiter", "detained", "items", "promote", "covenant", "nought",
"interposed", "seizing", "sinner", "vigor", "devote", "decorated", "sentimental", "yoke", "properties", "warlike", "perilous", "threats", "kindled", "lays", "hostility", "dragging", "mare", "regulations", "obstacle", "sage",
"destitute", "pays", "sleepy", "posterity", "nod", "mason", "patriotic", "plantation", "pitiful", "foster", "requisite", "expose", "oxen", "patch", "interruption", "lance", "payable", "definition", "birthday", "thumb",
"wolves", "hammer", "overwhelming", "intensely", "revolutionary", "fragrant", "bleeding", "sheltered", "circuit", "dominions", "sales", "energetic", "insignificant", "repetition", "amazing", "trains", "skirts", "tip", "trivial", "kick",
"tended", "rejoicing", "dig", "pet", "skull", "lectures", "ness", "threat", "legislature", "plunder", "removing", "jungle", "ghosts", "numbered", "famine", "palaces", "sorrowful", "improvements", "fuller", "asp",
"blocks", "darted", "shrine", "heel", "typical", "throws", "fortunately", "recognise", "fuel", "tranquil", "frown", "destination", "plunge", "moor", "pin", "mars", "associate", "vicious", "framed", "banquet",
"expressive", "instinctively", "lighting", "scanning", "subordinate", "jaws", "patent", "courtyard", "gulf", "destroying", "detailed", "regulating", "closet", "compel", "inland", "excepting", "pretext", "legislative", "stationed", "rash",
"margin", "champion", "settling", "billion", "shorter", "betwixt", "admiring", "nick", "chemical", "chapters", "worthless", "aristocratic", "especial", "hon", "attentive", "maintenance", "explanatory", "differently", "furiously", "pulse",
"scanty", "flee", "admiral", "clause", "resume", "compound", "pilot", "growled", "charmed", "imitate", "happening", "knot", "rags", "mock", "majestic", "messages", "suspense", "relationship", "skirt", "agency",
"arisen", "grin", "unusually", "hoarse", "mills", "intently", "dining", "demonstration", "depression", "lain", "expectations", "joining", "weekly", "trenches", "technical", "vehicle", "aimed", "borrow", "flattering", "prodigious",
"scope", "vegetation", "swelling", "fortified", "favoured", "salute", "topic", "blushed", "superb", "strengthen", "confidential", "crow", "shawl", "sunrise", "sings", "coats", "sturdy", "dissolved", "lifetime", "dispersed",
"sergeant", "contribute", "strode", "brigade", "verdict", "honors", "panting", "females", "richest", "attribute", "brighter", "hook", "discontent", "orderly", "airs", "tiger", "messengers", "penetrate", "identification", "holiness",
"crooked", "housekeeper", "productions", "prescribed", "rector", "spark", "sleeve", "honored", "tame", "highway", "militia", "nobleman", "energies", "spacious", "tearing", "affliction", "photograph", "ally", "ascent", "ditch",
"fishes", "rubbing", "tract", "standards", "afore", "ribbon", "integrity", "plus", "transparent", "farms", "pulpit", "ropes", "nineteen", "rescued", "counting", "perfume", "hounds", "solicited", "bother", "fascinating",
"qualified", "desolation", "essay", "rains", "renew", "odious", "assuredly", "suggests", "rider", "loneliness", "pond", "activities", "dazzling", "leaping", "squadron", "bowing", "novelty", "wrist", "keeper", "homeward",
"finely", "efficiency", "marvel", "tranquillity", "charities", "condemn", "elephants", "elders", "tries", "sweetly", "endurance", "bags", "reared", "jaw", "unique", "navigation", "inevitably", "admirably", "sect", "drum",
"poles", "verge", "piercing", "sanction", "forlorn", "approbation", "organic", "allegiance", "bin", "expressly", "ingenuity", "dispose", "stained", "theology", "withal", "duration", "fundraising", "collecting", "weigh", "sweetest",
"float", "consul", "monastery", "raging", "publish", "knocking", "precaution", "privately", "endeavored", "insight", "definitely", "stature", "oblige", "urging", "shift", "mould", "courses", "countless", "associates", "hymn",
"rapture", "tonight", "trumpet", "entrusted", "firmness", "comic", "breeding", "ken", "questioning", "factor", "monuments", "loveliness", "handled", "communities", "saloon", "stumbled", "witch", "confronted", "traveling", "seamen",
"backed", "profoundly", "gladness", "pomp", "mess", "practise", "sanctuary", "superstitious", "casual", "analyzed", "historic", "bored", "shrink", "judging", "treating", "expenditure", "encouraging", "diplomatic", "forcing", "studio",
"exposure", "crude", "compilation", "eve", "ascend", "unbroken", "countess", "binding", "exceed", "frail", "champagne", "shuddered", "carter", "mule", "inserted", "parson", "rascal", "inspire", "banner", "divorce",
"treacherous", "nineteenth", "invalid", "weaker", "organizations", "bolt", "ticket", "backwards", "captivity", "lame", "provoked", "vein", "lists", "gallop", "communications", "dagger", "passive", "shoe", "thrice", "corrected",
"mystic", "infancy", "foam", "tavern", "fraud", "cradle", "rifles", "vigorously", "censure", "gentleness", "sobbing", "monotonous", "explosion", "catastrophe", "respectfully", "wearied", "cats", "blamed", "needful", "fireplace",
"gravel", "affords", "discovering", "jar", "selfishness", "tolerably", "clerks", "ark", "moist", "wid", "sauce", "prompted", "exceptions", "bullets", "writ", "insolent", "moisture", "furnace", "healing", "fewer",
"deem", "apron", "humiliation", "punctuation", "rolls", "doe", "rotten", "richer", "behavior", "nowadays", "pamphlet", "loan", "beads", "divers", "unreasonable", "realise", "lust", "annually", "detach", "gaily",
"shares", "gifted", "planet", "feverish", "resurrection", "consecrated", "enforced", "shelf", "fan", "fluid", "brightly", "damsel", "kid", "frantic", "neatly", "anon", "ascribed", "insane", "tropical", "hardened",
"overthrow", "phase", "achievement", "immortality", "obscurity", "assumption", "discern", "hopeful", "humorous", "composure", "turf", "dame", "missionaries", "orator", "perpetually", "arbitrary", "ecstasy", "retirement", "pronounce", "authorized",
"familiarity", "hastings", "clubs", "reconciled", "grievous", "mercury", "elegance", "chivalry", "luminous", "beseech", "benevolent", "confided", "dances", "perplexity", "escaping", "terrific", "companionship", "commence", "daisy", "parliament",
"creep", "pleading", "disdain", "sympathies", "guides", "emergency", "parcel", "suicide", "replies", "drawer", "contribution", "supposition", "vii", "link", "homely", "pluck", "ruling", "statesmen", "printing", "synonymous",
"sinister", "advocate", "destructive", "environment", "blossom", "bridle", "yon", "waistcoat", "extends", "confirm", "listing", "solemnity", "projects", "reporter", "deprive", "detachment", "infernal", "traversed", "moss", "skilled",
"announce", "hateful", "fugitive", "coolness", "insurrection", "cum", "med", "coachman", "expend", "stepping", "resign", "despatch", "excluded", "reject", "tough", "plea", "fragment", "lacked", "balcony", "darker",
"mac", "fork", "flatter", "iniquity", "meditation", "disastrous", "stain", "patches", "hints", "ordained", "drinks", "whipped", "burial", "employee", "employer", "hypothesis", "steed", "width", "transaction", "victories",
"devout", "outrage", "vary", "attorney", "rouse", "doubled", "schooner", "flaming", "offend", "sheriff", "encamped", "magnificence", "vent", "politely", "vines", "flags", "necessities", "nobler", "accusation", "impulses",
"packet", "shabby", "irritated", "industrious", "classic", "ranch", "ascending", "cruelly", "happiest", "accuse", "insulted", "bridges", "players", "sixteenth", "solicitation", "embarked", "idol", "odds", "aims", "illuminated",
"enchanted", "adversary", "pie", "reflecting", "pension", "luxurious", "pigs", "choir", "tumbled", "conqueror", "irritation", "verb", "monkey", "acceptable", "dynasty", "accurately", "divinity", "signature", "heretofore", "hazard",
"stead", "attire", "fling", "marine", "occupations", "soothing", "devised", "singer", "spaces", "disguised", "antique", "orthodox", "poisoned", "dove", "gratification", "electricity", "alien", "sorely", "cracked", "supremacy",
"summon", "depressed", "sexes", "offerings", "pledged", "irony", "recourse", "tortured", "thickly", "correspondent", "sounding", "sombre", "brushed", "reasonably", "duel", "reluctantly", "implies", "cable", "ridden", "acre",
"grieve", "inquiring", "colonists", "republican", "illustrate", "gilded", "clumsy", "satin", "displeased", "odor", "clearer", "prairie", "feudal", "flint", "freshness", "nursery", "explanations", "adoption", "reluctance", "crosses",
"blushing", "imported", "notorious", "equipped", "sinful", "starving", "bedside", "sovereigns", "abrupt", "excused", "injure", "incessant", "correctly", "drooping", "adored", "embroidered", "pasture", "pillar", "import", "founder",
"torch", "vault", "worm", "bravery", "confinement", "trusting", "butler", "rattle", "transported", "estimation", "edit", "gotten", "cuts", "headlong", "outfit", "insolence", "secrecy", "thereupon", "unlucky", "eighth",
"valour", "grammar", "relaxed", "mentions", "adjacent", "knives", "attacking", "exceptional", "recollections", "deposit", "establishing", "muddy", "arches", "aspects", "senior", "fragrance", "colonial", "penetrating", "refinement", "yacht",
"intelligible", "stray", "forcibly", "superficial", "tends", "identified", "wan", "choosing", "frighten", "grotesque", "reprinted", "tutor", "contributing", "gaiety", "besieged", "robbery", "transmitted", "swam", "consequential", "slid",
"stony", "gratify", "heavier", "confidently", "demon", "treatise", "mechanically", "batteries", "trading", "cock", "pilgrimage", "extinct", "idleness", "merrily", "excursion", "handling", "eminence", "lump", "boyhood", "superfluous",
"wee", "dome", "shivering", "accidental", "thickness", "continuance", "fixing", "rustic", "cheered", "premises", "delivery", "nodding", "snowy", "curved", "productive", "discouraged", "variations", "shilling", "swollen", "miraculous",
"stubborn", "drives", "orchard", "persuasion", "invaded", "ungrateful", "insensible", "muscle", "cultivate", "involuntarily", "speedy", "variation", "harp", "peaks", "daybreak", "magnitude", "precautions", "rub", "requiring", "coral",
"grapes", "fairest", "locality", "opponent", "bondage", "beans", "cowardly", "grandson", "nail", "protecting", "hospitable", "proving", "benevolence", "civilisation", "mounting", "desiring", "rushes", "precision", "watchful", "harness",
"perchance", "forbade", "channels", "indication", "zealous", "tact", "seventeenth", "stating", "toast", "dreadfully", "asterisk", "edifice", "swelled", "accomplishment", "sundry", "reckoning", "mouse", "prostrate", "helm", "slim",
"whistling", "syllable", "handwriting", "commissioners", "lime", "spur", "unfit", "relish", "reduction", "sown", "venetian", "cordially", "hush", "breasts", "slipping", "pat", "dialogue", "forwards", "entreat", "fascination",
"belly", "neutral", "grasping", "diligence", "disgusted", "retiring", "strokes", "sob", "vine", "compose", "icy", "inconvenience", "pots", "dimensions", "abused", "armor", "detect", "contradiction", "banker", "infamous",
"powerless", "passenger", "crust", "historians", "disclaim", "peculiarities", "sting", "simultaneously", "watches", "episode", "achieve", "populace", "incense", "persistent", "fruitful", "scoundrel", "coasts", "starve", "consultation", "habitation",
"goat", "howling", "tailor", "flourish", "trifles", "dashing", "disappearance", "sour", "practicable", "shameful", "inviting", "criminals", "leisurely", "accumulated", "audible", "topics", "expends", "radiance", "underline", "parade",
"spoils", "helmet", "consternation", "expenditures", "impose", "originator", "unequal", "wooded", "enduring", "valet", "proclaim", "impossibility", "territories", "deference", "ravine", "accommodation", "boyish", "spray", "theological", "anonymous",
"injurious", "formally", "sports", "scales", "discontinue", "calf", "manual", "disturbing", "potent", "anticipation", "melt", "tilde", "grade", "mischievous", "pang", "pathos", "alternately", "brisk", "stool", "justification",
"foreigner", "endeavouring", "satire", "delete", "masculine", "spies", "umbrella", "transportation", "yell", "remnant", "boot", "ignored", "thrilling", "ale", "mineral", "goose", "truce", "lastly", "airy", "sketches",
"groves", "comprehension", "cling", "duck", "abyss", "baffled", "planning", "abominable", "aversion", "drawings", "customers", "weird", "traveled", "incessantly", "flattery", "director", "improbable", "moderation", "awakening", "males",
"pairs", "temporal", "con", "nicely", "lapse", "vitality", "soap", "patriot", "malicious", "eyed", "pirates", "enforce", "doll", "briskly", "sez", "skeleton", "comprehensive", "buttons", "crushing", "personages",
"threaten", "nuts", "undone", "wright", "frankness", "hides", "progressive", "villa", "resource", "confine", "sewing", "congratulate", "reconcile", "insurance", "terminated", "dusky", "appoint", "pearl", "thrilled", "gains",
"interrupt", "extravagance", "jokes", "suppress", "quod", "signify", "layer", "clue", "kettle", "contemplate", "aforesaid", "tooth", "sensibility", "boldness", "mature", "tolerable", "rabbit", "befallen", "needless", "awaken",
"clasp", "lets", "blinded", "conductor", "dependence", "guarantee", "affectionately", "player", "wires", "thicket", "walker", "outstretched", "procedure", "wheeled", "aye", "oneself", "recommendation", "projecting", "shriek", "futile",
"cheerfulness", "deity", "fifteenth", "gap", "muscular", "dripping", "insect", "values", "brooding", "restaurant", "baptism", "imaginative", "rhyme", "exhaustion", "intrigue", "senseless", "yearly", "baron", "occupying", "imply",
"absurdity", "launched", "resolutely", "vowed", "attach", "characterized", "fellowship", "posture", "caps", "demanding", "owl", "beset", "ensuring", "suite", "thereto", "heaped", "jewel", "regained", "voluntarily", "longitude",
"permanently", "jumping", "babe", "secondly", "violin", "rogue", "rainy", "reconciliation", "emotional", "radical", "accursed", "tendencies", "concrete", "resident", "lustre", "hull", "ominous", "overboard", "uproar", "cavern",
"combine", "respectively", "menace", "pilgrims", "peak", "currency", "silken", "violet", "mastery", "objective", "plucked", "litter", "memorial", "bids", "fondly", "clapped", "tariff", "beneficial", "unsolicited", "reluctant",
"separately", "patronage", "revenues", "dragon", "mike", "ranges", "vexation", "indicates", "overheard", "tray", "thereafter", "exporting", "mound", "taxation", "frenzy", "horizontal", "thirsty", "disputed", "charter", "redistribution",
"boasted", "item", "termination", "eminently", "suggestive", "linger", "shady", "calculation", "expansion", "mast", "confer", "commanders", "pitied", "twist", "traditional", "involve", "interfered", "wanton", "repay", "routine",
"groom", "solve", "grassy", "tempt", "unsuccessful", "witty", "politician", "yearning", "lid", "noticing", "courtiers", "cheering", "bounty", "consequent", "renown", "regulation", "fowl", "mayor", "wrinkled", "defy",
"threads", "violation", "junction", "boss", "particles", "glories", "signifies", "constrained", "paternal", "piles", "hardware", "engaging", "peer", "counties", "mocking", "avoiding", "rebuke", "abolished", "cheers", "idiot",
"morbid", "wrung", "outcome", "gilt", "coldness", "applying", "strand", "renowned", "fishermen", "creative", "circus", "moustache", "proverb", "lowering", "biggest", "sly", "nursing", "boon", "weighing", "brink",
"degraded", "avenge", "hum", "minority", "ridges", "perils", "merchandise", "aloof", "despairing", "acquisition", "asylum", "chickens", "placid", "affirm", "trod", "gardener", "schedule", "calmness", "protector", "concealment",
"trench", "fore", "accession", "dey", "connexion", "mend", "considers", "municipal", "achievements", "cherish", "deserving", "exert", "riot", "veteran", "advancement", "inventor", "meek", "hopelessly", "judicious", "tending",
"testify", "governess", "orchestra", "garb", "condemnation", "foregoing", "bacon", "maternal", "wasting", "strata", "hushed", "sculpture", "miniature", "corrections", "tangled", "completion", "regulated", "flavor", "brand", "intimately",
"unlimited", "dipped", "luggage", "inconsistent", "forsaken", "feebly", "woven", "rubbish", "tool", "spirited", "christendom", "chaos", "twinkling", "muffled", "accents", "accidentally", "degradation", "emancipation", "prosecution", "outbreak",
"defending", "dwarf", "abundantly", "turner", "disadvantage", "abolition", "disregard", "deliberation", "filthy", "notifies", "dealings", "demonstrated", "paced", "tense", "drums", "interpreter", "vanish", "astray", "hen", "workman",
"asunder", "baked", "bustle", "winged", "mentioning", "pastoral", "fabric", "trim", "musician", "patty", "mentally", "wrecked", "discreet", "apostle", "ledge", "roast", "accessed", "preface", "convincing", "quiver",
"stocks", "mourn", "commented", "redistribute", "precipice", "outdated", "dialect", "elementary", "freight", "cowardice", "wipe", "deserts", "shelves", "denial", "traits", "denounced", "underground", "phantom", "whirling", "pecuniary",
"dire", "hostilities", "gait", "vividly", "instruct", "dickens", "puritan", "clutched", "acknowledgment", "conjunction", "oppressive", "intermediate", "formula", "sneer", "ore", "plentiful", "plump", "combinations", "purest", "cheat",
"doubly", "inadequate", "blest", "forbear", "haunt", "treaties", "fearless", "constable", "enveloped", "enmity", "bridegroom", "curate", "developing", "frock", "mining", "audacity", "improper", "motto", "faction", "architect",
"melting", "delicately", "register", "heroine", "indefinite", "console", "defensive", "perceptible", "fruitless", "ransom", "surplus", "solicitude", "effectual", "shiver", "gal", "wed", "contemptuous", "plough", "snakes", "felicity",
"reef", "outset", "constitutes", "lament", "tissue", "draft", "impelled", "epic", "fisherman", "obstinacy", "lemon", "hound", "measuring", "conscientious", "robber", "toy", "impart", "statute", "girdle", "basil",
"rebellious", "stair", "biting", "consulting", "perseverance", "manila", "massacre", "cough", "blazed", "transition", "button", "headache", "tenant", "burns", "harmonious", "dreamy", "burgundy", "collections", "unkind", "inscribed",
"cushions", "programme", "din", "laborious", "manufacturing", "markets", "zone", "humane", "fertility", "languid", "ninth", "curses", "introducing", "alcohol", "impending", "declining", "advantageous", "heal", "millennium", "staid",
"planting", "theatrical", "spectator", "greedy", "commonwealth", "suffrage", "tremulous", "commodities", "stuffed", "admitting", "aching", "ninety", "discomfort", "imperative", "bobby", "bachelor", "geographical", "longest", "courageous", "carpenter",
"concluding", "danish", "steer", "influential", "surround", "random", "ounce", "afresh", "dictated", "ruddy", "rusty", "drown", "slide", "sow", "appalling", "profess", "sickly", "rides", "spoon", "imminent",
"dominant", "leadership", "pinch", "wearily", "ducks", "diary", "duchess", "regain", "rum", "churchyard", "fondness", "apprehend", "ordinarily", "quicker", "thereon", "balloon", "individuality", "securely", "connecting", "celebrate",
"bluff", "dawned", "amiss", "chalk", "sticking", "fuss", "dazed", "deputy", "forsake", "automobile", "discussions", "refreshment", "amendment", "appealing", "vertical", "insufficient", "hem", "gorge", "baptized", "damn",
"silvery", "pastor", "inherent", "preventing", "inference", "advertisement", "mutton", "packing", "enclosure", "theft", "publisher", "spontaneous", "rats", "apparition", "refreshing", "sweetheart", "renounce", "lifeless", "adore", "vinegar",
"uncovered", "utility", "orphan", "symbols", "gracefully", "mightily", "peculiarity", "ash", "floods", "partake", "contemptible", "deities", "profane", "foreseen", "conceit", "commend", "twelfth", "manifestation", "revive", "prone",
"connect", "princely", "overtake", "improving", "downwards", "ferocious", "intervention", "subsistence", "susceptible", "tunnel", "disciple", "revival", "twins", "ivy", "puzzle", "citadel", "temporarily", "despotism", "internet", "mechanism",
"stoop", "directors", "mathematics", "raft", "fade", "soothe", "pork", "substituted", "physically", "brilliancy", "dot", "loaf", "expanse", "shocking", "rudely", "isle", "balanced", "extracted", "fable", "matches",
"index", "cigars", "liver", "transmit", "dispatch", "onto", "veranda", "dip", "inexplicable", "liar", "diminish", "dungeon", "unit", "pagan", "brig", "monopoly", "rim", "sordid", "complaining", "temperate",
"chat", "gambling", "maps", "amber", "trot", "howl", "shipping", "ton", "magazines", "bricks", "submarine", "semblance", "perpendicular", "regardless", "fervent", "sane", "wreath", "animation", "earthquake", "sloping",
"smoothly", "tension", "intrigues", "fearfully", "macaulay", "laboratory", "cork", "comments", "whale", "wedded", "whiteness", "convicted", "deception", "paved", "scruple", "paintings", "therewith", "religions", "governing", "colleagues",
"shrinking", "tickets", "prophetic", "undergo", "hare", "haze", "poisonous", "omit", "beware", "sagacity", "concession", "worker", "ted", "incline", "caste", "leapt", "dissatisfied", "hardest", "toilet", "buddha",
"offense", "woodland", "gentry", "starvation", "grudge", "penance", "tips", "rooted", "outburst", "fortitude", "turk", "devour", "malignant", "accorded", "brandon", "anticipate", "speechless", "inquisition", "eccentric", "anecdote",
"annals", "scrutiny", "burroughs", "rhythm", "discord", "marius", "diversion", "archie", "rat", "knit", "correspond", "detain", "dis", "esp", "interpret", "vehement", "soda", "naughty", "salon", "operate",
"idly", "imperious", "candid", "blooming", "wharf", "disgraceful", "stunned", "redemption", "drain", "wage", "cooper", "embassy", "unfinished", "nasty", "impetuous", "cemetery", "oblivion", "prohibited", "breeches", "abound",
"frivolous", "essays", "plaster", "tap", "chairman", "dismiss", "provoke", "reside", "deficient", "decoration", "heroism", "toe", "wade", "apparel", "hazel", "inability", "farthest", "invent", "knave", "twain",
"carelessness", "affectation", "connections", "climax", "avowed", "industries", "brood", "tempting", "define", "antwerp", "forefathers", "stretches", "gratifying", "plight", "restricted", "cupboard", "ludicrous", "alms", "colossal", "stupidity",
"monotony", "stimulus", "vigilance", "digest", "vale", "overcoat", "colorado", "wink", "nous", "rack", "incomprehensible", "antagonist", "methinks", "barley", "plateau", "superintendent", "indescribable", "expanded", "presentation", "archbishop",
"devise", "rubber", "adieu", "exclude", "carts", "lone", "whisky", "abuses", "inflict", "nightfall", "counts", "chocolate", "privileged", "hermit", "exultation", "overtook", "coincidence", "scratch", "screw", "caravan",
"divert", "eliza", "comparing", "hood", "explore", "glove", "chaste", "whirl", "adventurous", "skipper", "tiresome", "implements", "recompense", "plank", "insure", "laboured", "exaggeration", "shepherds", "lilies", "ballad",
"befall", "cylinder", "teddy", "summary", "daresay", "photographs", "colleges", "dissolution", "geneva", "marches", "instituted", "seals", "vehemence", "chaplain", "knots", "wail", "kneel", "unlikely", "deceit", "challenged",
"geography", "herald", "lowly", "peep", "swarm", "clarke", "joyfully", "engraved", "bowels", "purposely", "blindness", "systematic", "virtually", "conformity", "remedies", "maxim", "indexes", "marshall", "baking", "invincible",
"impertinent", "bust", "visage", "intuition", "mingle", "bathing", "arched", "investment", "tabernacle", "client", "ghostly", "furs", "catalogue", "dock", "tenor", "arouse", "verbal", "excessively", "brazil", "strenuous",
"irishman", "recess", "unclean", "psalms", "analogy", "chemistry", "peninsula", "infer", "maritime", "secular", "hawk", "rein", "averted", "bake", "constantine", "oracle", "alley", "softness", "pierce", "spinning",
"snatch", "manufactured", "launch", "psychology", "worms", "regulate", "farming", "fasten", "actress", "etiquette", "theater", "thanksgiving", "valor", "untouched", "tactics", "drug", "adverse", "gaunt", "conducting", "veritable",
"overtaken", "distorted", "rosa", "nina", "quart", "caprice", "candy", "obliging", "planets", "soothed", "sic", "opium", "pavilion", "strait", "sanguine", "cords", "odour", "trout", "paste", "regularity",
"metallic", "scrap", "convict", "instructive", "investigate", "celtic", "package", "pirate", "fiend", "moan", "revealing", "trades", "rounds", "accomplishments", "crawl", "aft", "prevalent", "role", "dose", "evans",
"hypocrisy", "salmon", "snap", "alma", "magical", "tire", "hetty", "impenetrable", "geese", "madly", "manifold", "noticeable", "pudding", "volcanic", "locke", "magnetic", "deals", "core", "decency", "observance",
"durst", "scratched", "predecessor", "diplomacy", "wert", "impartial", "disinterested", "wig", "pump", "swedish", "norfolk", "reigns", "similarly", "reap", "dam", "facilities", "slippery", "transformation", "oxygen", "suburbs",
"dares", "ornamental", "pondered", "fringe", "raiment", "henrietta", "wellington", "foreman", "feat", "thirteenth", "sultan", "certificate", "rue", "heresy", "arabia", "medal", "location", "ether", "ruby", "heading",
"subdue", "adorn", "ancestor", "warmer", "cluster", "quotation", "fullest", "exposition", "custody", "thermometer", "plausible", "toss", "desperation", "rhetoric", "scornful", "bailey", "rung", "civility", "dingy", "scaffold",
"concentration", "avarice", "scrape", "pools", "oar", "flutter", "martyr", "handy", "montague", "bait", "login", "commotion", "congenial", "drawers", "telescope", "deposits", "edwards", "craving", "bureau", "oscar",
"speculative", "huddled", "diverse", "slice", "renaissance", "angelo", "meg", "murderous", "serenity", "perspiration", "coventry", "impudent", "ardor", "necklace", "alight", "stimulated", "clifford", "steadfast", "genoa", "courier",
"inflamed", "drill", "spelling", "respond", "seriousness", "fourteenth", "womb", "literal", "singers", "usefulness", "cloudy", "mortality", "profusion", "fleeting", "twentieth", "maturity", "surf", "weed", "phases", "overcame",
"womanhood", "envious", "tapped", "latent", "whiskey", "relatively", "forbidding", "cleopatra", "willow", "mathematical", "sojourn", "booty", "camel", "implore", "amateur", "morally", "qualifications", "gasp", "gliding", "tested",
"racing", "brightest", "joel", "extremes", "damascus", "labored", "peggy", "exit", "originality", "humming", "isolation", "sometime", "glee", "adult", "solace", "biography", "hardship", "lied", "donkey", "trader",
"rumour", "amply", "confide", "favors", "perspective", "belgian", "withstand", "robust", "pro", "val", "eats", "snare", "monthly", "wines", "ignore", "envoy", "flown", "reverie", "jehovah", "contrive",
"chatter", "judas", "nourishment", "reforms", "clatter", "allude", "corrupted", "thorn", "junior", "tony", "psychological", "constancy", "misunderstood", "signals", "drying", "harshly", "distressing", "novelist", "editors", "intricate",
"limestone", "collision", "pebbles", "knitting", "ordeal", "foresee", "peas", "repast", "supplying", "clan", "abject", "dart", "berth", "bridal", "indirect", "unnoticed", "tint", "insults", "precedent", "twisting",
"bully", "vacation", "canon", "aisle", "click", "inspiring", "oval", "impracticable", "delirium", "cedar", "contradict", "ingratitude", "soften", "bewilderment", "servitude", "comely", "stump", "redeem", "spun", "elastic",
"poultry", "horseman", "dictionary", "prettiest", "adoration", "wager", "involving", "pathway", "wistful", "advent", "gear", "celebration", "conceivable", "drowning", "faintest", "acquiring", "befell", "wares", "rendezvous", "snug",
"watery", "accompaniment", "chaps", "crawling", "lumber", "publishing", "customer", "mediaeval", "prints", "lavish", "genesis", "rug", "analogous", "eleventh", "galley", "partition", "blunder", "fanciful", "ham", "rainbow",
"sentinel", "hereby", "outlook", "smitten", "unmarried", "mice", "installed", "vivacity", "marking", "aesthetic", "consume", "resent", "pose", "contentment", "sterling", "veneration", "barking", "bower", "organism", "unintelligible",
"emphatic", "occurring", "factors", "guise", "editorial", "impudence", "midday", "corporal", "aright", "nigger", "lily", "noun", "scout", "spit", "cursing", "manifestly", "battalion", "heritage", "brotherhood", "nun",
"wad", "folding", "discerned", "powerfully", "helpful", "persist", "frigate", "spotted", "atoms", "curves", "outlet", "erroneous", "violated", "withheld", "fairies", "inherit", "sledge", "taller", "supervision", "butt",
"handsomely", "tank", "velocity", "arctic", "colleague", "pins", "butcher", "drowsy", "butterfly", "chart", "twin", "sunken", "exasperated", "narrowly", "insulting", "deficiency", "operating", "overthrown", "gallows", "diligent",
"hindu", "blunt", "omen", "bleak", "vehemently", "wretchedness", "ensure", "denotes", "sentenced", "unfair", "encampment", "possessor", "absorbing", "descendant", "sub", "drugs", "engineers", "independently", "bucket", "clerical",
"ache", "glitter", "ordinance", "bamboo", "vocation", "admirer", "limp", "pallid", "mildly", "organisation", "dealer", "auspicious", "deuce", "emblem", "primarily", "reducing", "ritual", "decorations", "thigh", "groaning",
"scant", "fiscal", "mien", "charging", "cor", "railing", "peers", "inferred", "sanctity", "accumulation", "cynical", "inspector", "wardrobe", "texture", "adjustment", "epistle", "adventurer", "priesthood", "seaman", "turbulent",
"chant", "marsh", "unaware", "vase", "initial", "baths", "weighty", "minimum", "correction", "morsel", "overlook", "meagre", "unanimous", "magician", "mystical", "inhabit", "shaggy", "unaccountable", "nightmare", "carbon",
"coil", "lawless", "stairway", "willingness", "sarcasm", "crisp", "jerk", "tout", "vocabulary", "stroll", "poorly", "composing", "parliamentary", "controlling", "fitness", "thoughtless", "temperance", "illumination", "translations", "martyrdom",
"mellow", "nationality", "jam", "austere", "shoots", "casually", "pensive", "flavour", "nets", "dice", "satisfactorily", "shrunk", "administer", "ante", "swine", "listener", "unimportant", "genera", "contrivance", "deplorable",
"museum", "benefactor", "tints", "alphabet", "rout", "scatter", "extant", "misunderstanding", "universities", "dexterity", "rag", "inseparable", "punch", "brazen", "economical", "pernicious", "mythology", "drained", "bolted", "stride",
"circumference", "meddle", "axis", "gum", "las", "kinder", "closes", "ferocity", "giddy", "secluded", "resisting", "satisfying", "reliable", "disgusting", "ethical", "crouching", "lash", "recital", "collapse", "unsatisfactory",
"lore", "varies", "mainland", "repute", "cushion", "confound", "scrub", "myth", "flights", "oats", "layers", "ownership", "cape", "glimmer", "scare", "waked", "scrupulous", "equals", "redress", "brake",
"nut", "stability", "crafty", "kirk", "bough", "momentous", "albeit", "enlarge", "hardness", "civilised", "dotted", "defiant", "timidity", "solar", "heartless", "mat", "shun", "raid", "disclose", "suppression",
"puff", "juncture", "beak", "unjustly", "foresaw", "rot", "aggressive", "predicted", "grate", "lease", "ponderous", "maketh", "repaid", "charcoal", "chilly", "arrogance", "friction", "participation", "pier", "stale",
"intoxicated", "commissioned", "ratio", "comb", "masterpiece", "wholesale", "embraces", "trodden", "translate", "mortar", "recreation", "rite", "truthful", "cavalier", "caress", "curling", "rivalry", "whim", "abreast", "peg",
"pestilence", "ceremonial", "receiver", "sample", "distinctive", "consummate", "matron", "claiming", "plural", "initiative", "inexhaustible", "spider", "reed", "streak", "blocked", "smashed", "populous", "baronet", "commodore", "jelly",
"advocates", "salutation", "mutiny", "chronicles", "comforting", "serviceable", "parchment", "playful", "potato", "transient", "administrative", "anarchy", "barber", "revision", "operated", "farce", "germ", "profile", "provides", "noting",
"disordered", "menacing", "heightened", "finance", "averse", "azure", "bathe", "campaigns", "lessen", "slate", "acquaint", "gin", "humiliating", "cleft", "conveyance", "chivalrous", "capricious", "tribune", "pilgrim", "entreaty",
"womanly", "paltry", "sporting", "maker", "digestion", "infamy", "lambs", "gaping", "periodical", "standpoint", "amorous", "tub", "luxuriant", "basic", "mutually", "greed", "premature", "extinction", "boiler", "intimation",
"scandalous", "separating", "oratory", "banish", "electrical", "herb", "multiply", "prosper", "friar", "nightly", "ole", "monkeys", "interminable", "enjoys", "similarity", "riddle", "cleaning", "subscription", "copious", "exclaim",
"forged", "voting", "scourge", "darkly", "privacy", "arena", "bearded", "alacrity", "sensual", "spin", "neutrality", "flannel", "fasting", "trailer", "avert", "trustworthy", "unchanged", "traveler", "unfamiliar", "puffed",
"mirrors", "conform", "particle", "railways", "stupendous", "paddle", "innate", "reformation", "volley", "statistics", "agrees", "simpler", "padre", "congratulations", "lids", "muse", "inhabitant", "rustle", "clump", "calendar",
"flute", "inaccessible", "yore", "jay", "repulsive", "fray", "nomination", "conclusive", "peaceable", "inconceivable", "emerald", "lava", "trillion", "uppermost", "arduous", "lyric", "downright", "reproduction", "foresight", "consistency",
"ape", "senators", "pallor", "span", "salad", "snuff", "drooped", "greetings", "chestnut", "inquisitive", "vicar", "attic", "savings", "affirmative", "ills", "applications", "dye", "nominal", "demonstrate", "dispense",
"dissatisfaction", "merciless", "trusty", "coloring", "perusal", "plaintive", "discarded", "precarious", "infection", "ruinous", "bolts", "arithmetic", "considerate", "lark", "ethics", "conventions", "stumbling", "pitcher", "slips", "officially",
"annoy", "glide", "impunity", "amends", "sol", "conveying", "abandonment", "mane", "tinge", "brim", "forenoon", "sparkle", "syllables", "shrug", "enchantment", "trait", "bribe", "composer", "preparatory", "audacious",
"outskirts", "soiled", "fiddle", "football", "partnership", "continuation", "pioneer", "vest", "bass", "derby", "quarry", "rigging", "dizzy", "abnormal", "omission", "idolatry", "sequence", "squeeze", "cabbage", "canopy",
"athletic", "drunkenness", "intrusion", "assign", "tackle", "dreamt", "sceptre", "exacting", "parched", "eddy", "percentage", "twinkle", "curb", "sandstone", "invaluable", "fathom", "preferable", "advertising", "scraps", "lever",
"muster", "cavity", "barbarian", "sleepless", "fried", "abstraction", "forefinger", "spade", "erection", "scorned", "pail", "withdrawal", "senator", "mortgage", "ancestral", "succour", "forbearance", "repress", "spouse", "valid",
"witchcraft", "workmanship", "legacy", "proximity", "incorporated", "muzzle", "clusters", "valve", "compelling", "dissipated", "flickering", "guinea", "sup", "tarry", "derision", "vehicles", "accommodate", "glossy", "iris", "relic",
"ant", "heath", "bug", "vocal", "downfall", "construct", "undue", "vapor", "bat", "whimsical", "contradictory", "unlocked", "foretold", "automatic", "explicit", "indolent", "mates", "artful", "downcast", "ordinances",
"effectively", "missions", "stalk", "indistinct", "pregnant", "reddish", "coveted", "fret", "peeping", "buck", "sumptuous", "indefinitely", "reliance", "cocked", "dad", "everyday", "intoxication", "aghast", "subterranean", "turmoil",
"forfeit", "chasm", "inspect", "perverse", "precipitate", "ambush", "evermore", "blot", "nook", "verdure", "parapet", "cessation", "ankle", "classification", "fervently", "oddly", "haul", "embarrassing", "hairy", "northwest",
"disabled", "laurel", "arrogant", "hurts", "demonstrations", "splash", "curl", "livelihood", "wary", "scattering", "brace", "converts", "detestable", "abandoning", "weakly", "clothe", "gem", "tremor", "surveying", "variable",
"anniversary", "wrap", "curly", "diversity", "prestige", "desertion", "freezing", "heedless", "sentry", "believer", "ram", "rowing", "negligence", "sulphur", "discrimination", "cooling", "millionaire", "flowering", "meridian", "wins",
"awed", "beastly", "nuisance", "abstain", "continental", "stanza", "target", "unwonted", "whit", "stall", "sham", "dictate", "empress", "gout", "jobs", "manure", "sidewalk", "sate", "grievance", "axes",
"bony", "invest", "ebb", "rabble", "restlessness", "cruise", "rally", "rumor", "hysterical", "girlish", "actively", "shortest", "cheque", "disregarded", "retort", "rocking", "emerge", "perch", "flask", "countryman",
"lonesome", "manned", "unarmed", "wast", "frog", "unscrupulous", "yarn", "victuals", "outrageous", "appropriation", "foolishness", "quickness", "adversity", "diseased", "salutary", "smelt", "territorial", "hurricane", "irons", "canyon",
"indulging", "vapour", "disobedience", "atrocious", "leaps", "tapestry", "provocation", "impotent", "smite", "acquitted", "tumultuous", "barge", "palpable", "apprentice", "lances", "compartment", "godly", "sarcastic", "therefrom", "specifically",
"uniformity", "emerging", "atonement", "whereabouts", "framework", "sponge", "mountainous", "annoying", "cot", "squirrel", "wand", "grind", "bang", "unreal", "blacksmith", "injunction", "scarcity", "withhold", "outright", "cement",
"growl", "aggregate", "fraction", "exaltation", "inexorable", "jug", "purer", "sap", "illegal", "presses", "stealthily", "dissolve", "volcano", "equilibrium", "obstinately", "sullenly", "assassination", "commissions", "respectability", "bases",
"resounded", "closest", "embroidery", "gunpowder", "reproof", "combining", "weaving", "earnings", "indoors", "manufacturers", "pitiless", "scarf", "picnic", "misled", "pompous", "respite", "exploit", "tracing", "geological", "passport",
"confines", "dishonour", "executioner", "township", "vacancy", "acquiescence", "crumbling", "exploration", "needy", "stationary", "disconcerted", "wanderer", "beaver", "lookout", "onion", "depicted", "boisterous", "couples", "speakers", "woollen",
"lightness", "bitten", "aux", "toleration", "scar", "bohemian", "vested", "affinity", "sous", "penitent", "abiding", "immoral", "dishonest", "yawning", "mustache", "supplement", "whirlwind", "clash", "lamentable", "farthing",
"speck", "biscuit", "appellation", "reserves", "uncouth", "birch", "armchair", "greasy", "leaden", "dough", "lining", "cleverness", "ascetic", "clutch", "embark", "quotations", "friendliness", "liberally", "trance", "untrue",
"rejection", "grating", "inexperienced", "mon", "wintry", "stalwart", "meats", "stamping", "variance", "apiece", "firmament", "absorption", "apprehensive", "terminate", "wilful", "conveniently", "cleanliness", "collective", "filth", "timely",
"herein", "ignoble", "canton", "lamentations", "ware", "adjective", "glen", "invade", "livid", "buggy", "prolong", "weaken", "folio", "dismissal", "quay", "enchanting", "heave", "purified", "significantly", "experimental",
"film", "repressed", "cooperation", "sequel", "wench", "calves", "pence", "kindle", "southwest", "roam", "distraction", "havoc", "lunatic", "soldiery", "tablet", "unwise", "assassin", "awkwardly", "verandah", "dejected",
"publicity", "suspension", "throb", "relaxation", "ardently", "cove", "tan", "unhappiness", "expand", "stronghold", "laughingly", "tumble", "colder", "forge", "covert", "imprudent", "lazily", "quotes", "impulsive", "degenerate",
"underlying", "leafy", "chum", "sweeps", "abashed", "loathsome", "remnants", "darts", "indolence", "affront", "fossil", "adhered", "atom", "constraint", "themes", "mechanics", "pauses", "childlike", "vigilant", "broth",
"boar", "irritable", "epithet", "overseer", "aurora", "mire", "ineffectual", "sagacious", "incomparable", "triple", "vanilla", "initiated", "aids", "zest", "levity", "fastidious", "pyramid", "cycle", "nap", "northeast",
"unlawful", "crater", "hamlet", "shrub", "births", "paw", "boom", "dint", "fervour", "reproduce", "prominence", "abated", "aerial", "cloister", "traverse", "cutter", "forthcoming", "shyness", "spotless", "boasting",
"congress", "overgrown", "sanctified", "haven", "engraving", "equity", "eclipse", "visionary", "archives", "grape", "hindrance", "omnibus", "rocked", "implacable", "sweets", "workshop", "bayonet", "corresponds", "lute", "decrease",
"broom", "incur", "misgivings", "moderately", "grease", "antagonism", "plumage", "slander", "naming", "citizenship", "pronunciation", "judgement", "wearisome", "tangle", "matched", "angelic", "leant", "sepulchre", "concentrate", "anchorage",
"versed", "drivers", "clover", "pastime", "retains", "vineyard", "reverently", "spruce", "edict", "undo", "ferry", "panel", "repugnance", "impious", "relentless", "steaming", "mace", "fictitious", "objectionable", "shameless",
"anatomy", "dawning", "superman", "ruffian", "genteel", "preoccupied", "astronomy", "straightforward", "indefatigable", "maze", "descriptive", "prerogative", "des", "heather", "mar", "rupture", "submissive", "faithless", "scholarship", "candour",
"hazardous", "blockade", "deluge", "sickening", "outcry", "scold", "altitude", "turtle", "bide", "efficacy", "adviser", "cumulative", "pyramids", "stew", "commissioner", "actuated", "fibre", "joyously", "continuously", "cricket",
"denote", "pans", "recite", "unfavourable", "parable", "thrive", "pigeon", "extensively", "adaptation", "export", "peal", "incidentally", "offender", "dubious", "yawned", "ruthless", "trips", "disapproval", "operator", "courtier",
"par", "shaved", "abolish", "whosoever", "adjust", "miscellaneous", "unawares", "deceitful", "creates", "membership", "gauge", "terrestrial", "tolerate", "veterans", "kinsmen", "bard", "pike", "spartan", "badge", "hip",
"vex", "adhere", "sans", "stimulate", "ado", "superseded", "sash", "formality", "triangle", "compulsory", "conflagration", "treasurer", "gust", "fabulous", "despotic", "authoritative", "hydrogen", "tearful", "abounding", "fragile",
"excel", "spoilt", "bailiff", "laurels", "printer", "choke", "clustered", "monastic", "repel", "belongings", "credulity", "hapless", "unrest", "glimmering", "aspire", "contingent", "dowry", "chauffeur", "partiality", "chastity",
"cologne", "papal", "vogue", "flowery", "chronic", "peremptory", "confessor", "eke", "grounded", "verbs", "abbot", "mahogany", "runaway", "cope", "dilemma", "slack", "gaudy", "melodious", "contraction", "gypsy",
"prodigal", "momentarily", "tangible", "viceroy", "journalist", "bouquet", "symptom", "dangling", "bicycle", "homestead", "youngster", "compulsion", "sensuous", "sluggish", "infirmity", "festivities", "observant", "raven", "precedence", "shewn",
"lure", "precept", "winters", "nave", "droll", "bewildering", "tattered", "provisional", "outlaw", "priceless", "chieftain", "tar", "astounded", "vindictive", "lamentation", "teams", "engineering", "spiral", "wayside", "midway",
"partisan", "undertone", "lull", "precipitous", "hypocrite", "aperture", "stratagem", "expulsion", "commentary", "olden", "transmission", "gall", "refresh", "camera", "clamor", "intruder", "ripple", "apologize", "chuckle", "ugliness",
"basement", "lyre", "pondering", "spire", "pounded", "tributary", "locomotive", "celebrity", "insurgents", "quid", "accosted", "ballot", "lick", "discovers", "venison", "accomplice", "hilt", "pedestal", "cant", "potential",
"incompatible", "memoirs", "rheumatism", "magnanimity", "dancer", "balm", "uninteresting", "fervor", "eligible", "conceited", "migration", "kitten", "loop", "persistence", "tying", "slang", "distasteful", "laborer", "prosecute", "consuming",
"licence", "unfold", "tottering", "fir", "relinquish", "unavoidable", "repeal", "bye", "ego", "mercantile", "notably", "jet", "violate", "tennis", "addicted", "completing", "manor", "prelate", "kids", "windy",
"liberated", "impertinence", "survival", "masonry", "demeanor", "forsooth", "agreeing", "carving", "asses", "boarding", "surrey", "tug", "penetration", "freeze", "matrimony", "labyrinth", "triumphal", "outcast", "weave", "deputation",
"auntie", "intact", "questionable", "inanimate", "incumbent", "tack", "terra", "apathy", "hatchet", "daytime", "organize", "parsons", "artifice", "surpass", "masked", "presumably", "abbe", "drought", "saddled", "parrot",
"labourer", "collector", "convoy", "incidental", "jovial", "spine", "chord", "bleed", "forgave", "dissipation", "echoing", "napkin", "importation", "fry", "dessert", "keel", "incredulous", "focus", "arc", "detection",
"summed", "nocturnal", "smash", "facilitate", "porcelain", "correctness", "lusty", "surly", "forgetful", "chaise", "checking", "err", "arid", "attends", "frequency", "loom", "plume", "chagrin", "mouthful", "admittance",
"wakes", "retinue", "hose", "overflow", "absently", "discredit", "stitch", "vie", "directory", "revolting", "loins", "legion", "hogs", "separates", "ascribe", "politic", "stab", "chop", "password", "rousing",
"decorum", "inflammation", "stark", "unutterable", "vestibule", "regal", "watchman", "advertised", "hemisphere", "loading", "heating", "abhorrence", "counsellor", "animosity", "depressing", "coup", "perforce", "dedication", "outwardly", "uncanny",
"compassionate", "shroud", "cupid", "dogged", "moth", "thinker", "ravages", "fraught", "budget", "pours", "sloop", "rigorous", "strategy", "blissful", "uphold", "quartz", "benediction", "chords", "belle", "civic",
"halves", "accusing", "atone", "atlas", "languor", "earthen", "expectant", "haughtily", "obnoxious", "twig", "paramount", "aiming", "portal", "sable", "expiration", "consort", "hooked", "insert", "listless", "furtive",
"pollen", "acetylene", "meditate", "scissors", "reefs", "parsley", "signing", "nimble", "stag", "flexible", "accomplishing", "coronation", "slab", "voters", "inscrutable", "promenade", "qualification", "crucifix", "switch", "aspiration",
"defile", "skillful", "windward", "theoretical", "towel", "abounds", "wrestling", "disorderly", "suspiciously", "oration", "decade", "kingly", "inflexible", "counterfeit", "multiple", "cripple", "healthful", "loaves", "bourgeois", "crystalline",
"debates", "primeval", "dejection", "bandage", "penal", "tidy", "abstracted", "blasted", "perverted", "patrol", "feud", "hallowed", "testing", "convulsive", "crackling", "hack", "teaspoon", "hoc", "scraped", "glacier",
"safeguard", "clap", "vagabond", "allowable", "emigration", "purport", "chaff", "follower", "rapt", "maple", "completeness", "rake", "lurid", "moat", "portico", "upheld", "impaired", "thankfulness", "vanishing", "emptiness",
"marrow", "suns", "halfway", "invariable", "tenure", "sheath", "memorandum", "absorb", "cone", "prediction", "pew", "planes", "educate", "retribution", "betimes", "unbearable", "enquiry", "latch", "symmetry", "untimely",
"niggers", "domination", "bothered", "corral", "upside", "uneven", "instrumental", "parental", "incoherent", "priestly", "sacrament", "chronicle", "participate", "cooler", "elevator", "manufacturer", "stealthy", "indomitable", "lateral", "defendant",
"nymph", "experts", "mosque", "postpone", "inter", "seizure", "glade", "exhaust", "loathing", "penitence", "oblong", "saddles", "highways", "shipwreck", "sprinkle", "chess", "dispensed", "nucleus", "disperse", "toll",
"pitiable", "casket", "cherry", "enlightenment", "starboard", "poignant", "astounding", "supple", "consummation", "drunkard", "usages", "afoot", "vicissitudes", "mercenary", "metaphor", "naturalist", "swarthy", "widespread", "presumptuous", "stumble",
"translator", "bluntly", "diminutive", "inferiority", "evade", "peach", "sonorous", "conjure", "truck", "sultry", "dogma", "finery", "tow", "debtor", "scalp", "surpassing", "terrifying", "signification", "diction", "sulky" };
		static int word_lib_frequency[number_of_words] = { 56271872, 29944184, 11073318, 10078245, 8799755, 7725512, 7097981, 6139336, 6048903, 5741803, 5202501, 4580906, 4346500, 4108111, 4015425, 3971997, 3894211, 3703342, 3415846, 3340398,
3323884, 3224178, 2832569, 2820265, 2637136, 2509917, 2472663, 2400858, 2357654, 2320022, 1980046, 1961200, 1899787, 1875351, 1703963, 1665366, 1658553, 1605908, 1602329, 1573117,
1571110, 1533530, 1508779, 1498473, 1476767, 1462382, 1449681, 1424595, 1414687, 1400642, 1373270, 1318367, 1317723, 1280625, 1273589, 1255636, 1210074, 1192154, 1188501, 1185720,
1136757, 1120163, 1117089, 1108116, 1093366, 1090510, 1084286, 1075612, 1056659, 1021822, 989808, 978575, 978196, 966602, 923053, 922130, 899673, 889691, 887917, 883846,
882331, 881975, 878621, 873144, 873007, 863478, 860027, 829823, 825485, 819684, 807600, 799596, 792925, 792220, 791686, 788124, 787455, 770603, 758856, 750750,
748992, 745230, 744168, 740270, 729829, 715780, 704444, 701741, 690186, 689376, 680833, 674623, 673810, 671168, 656486, 654512, 648227, 647788, 640067, 621773,
613918, 610168, 608042, 591821, 588615, 570877, 569459, 567559, 564674, 559207, 554003, 552668, 547420, 545832, 544251, 541692, 536737, 534154, 533985, 531797,
526137, 520922, 517564, 517557, 512481, 502146, 501669, 498040, 496496, 495834, 495189, 484878, 484599, 479849, 479034, 475277, 470313, 467157, 466338, 465587,
461472, 458312, 456267, 447884, 447491, 445602, 435059, 432637, 432491, 432016, 431120, 430389, 429972, 428871, 427021, 426104, 424696, 424320, 422407, 420788,
420044, 419315, 418862, 416463, 415355, 413499, 413101, 405810, 398727, 390988, 387790, 387549, 385303, 385081, 384156, 382266, 380328, 373898, 373813, 371831,
369761, 369036, 366719, 366630, 359970, 355857, 355656, 355467, 354212, 352828, 351253, 350537, 349925, 349691, 349685, 344377, 344141, 342388, 337544, 337367,
335210, 334332, 333883, 330567, 328630, 326524, 326391, 326386, 326243, 325793, 325791, 325719, 325327, 324509, 322870, 322643, 322465, 318937, 316490, 315642,
313167, 311818, 309653, 308025, 307506, 307223, 304955, 304735, 304240, 301283, 298191, 296798, 296795, 295916, 295745, 295078, 293866, 293593, 291647, 291427,
289997, 289304, 287876, 286847, 284431, 281965, 279864, 279821, 278870, 278365, 277682, 277227, 275990, 275607, 275221, 273746, 272173, 271641, 271588, 270503,
269414, 268542, 267878, 266672, 265947, 265138, 264662, 264606, 263613, 263585, 262689, 262571, 260746, 260596, 259916, 257964, 255885, 255789, 255212, 254373,
252812, 251252, 251163, 249677, 249533, 249063, 247480, 246792, 246251, 244426, 244138, 243712, 242753, 242269, 242201, 242178, 242099, 241091, 240652, 240280,
239735, 239025, 238734, 238281, 237964, 237481, 235474, 235012, 234544, 233771, 233197, 233158, 230819, 230401, 230300, 229940, 229913, 226953, 226661, 226391,
226216, 226205, 224811, 224569, 224326, 224049, 222546, 222315, 222264, 220598, 218623, 218510, 218118, 217840, 216356, 215147, 213257, 213124, 213072, 212548,
212369, 211722, 211668, 211299, 211043, 210031, 209099, 208344, 208199, 206740, 206329, 206216, 205687, 205653, 204955, 204686, 203956, 203331, 202041, 201451,
200224, 200145, 199969, 199658, 199560, 198566, 198187, 198128, 196739, 196729, 196458, 196252, 195905, 195594, 194903, 194298, 192763, 192719, 192229, 191272,
191148, 190649, 190563, 188915, 188316, 186609, 186256, 186136, 185862, 184612, 184074, 184068, 183059, 181866, 180998, 180061, 179739, 179347, 178877, 178452,
178084, 178001, 177232, 176635, 176467, 176320, 175980, 175520, 174236, 173897, 173544, 172990, 172901, 172692, 172467, 171690, 171309, 170962, 170695, 170143,
169913, 169470, 169086, 168723, 168545, 168408, 167805, 166395, 165752, 164850, 164703, 164326, 164147, 163819, 163514, 163341, 163269, 162869, 162687, 162605,
162328, 162154, 162105, 161953, 161549, 160626, 160547, 159794, 158664, 158344, 158204, 158200, 157578, 157416, 156851, 156616, 155743, 155480, 155472, 154977,
154840, 154582, 154536, 154261, 154046, 154001, 153940, 153830, 153829, 153299, 152972, 152889, 152625, 151661, 151651, 151612, 151043, 150281, 149690, 149601,
149323, 148795, 148744, 148124, 147435, 147401, 146483, 146062, 145998, 145817, 145490, 145440, 145393, 145244, 145131, 144925, 144688, 144022, 143658, 143625,
142952, 141958, 141745, 141597, 141588, 141246, 140679, 140667, 140608, 139711, 139167, 138288, 138228, 137871, 136616, 136507, 136296, 136265, 135512, 135386,
134689, 134351, 134257, 134075, 134048, 133975, 133771, 133663, 133159, 133084, 133030, 132785, 132560, 132368, 132288, 132096, 132045, 131617, 131509, 131397,
130867, 130590, 130394, 130376, 130255, 130079, 129644, 129586, 129473, 129411, 129325, 129276, 129116, 129075, 129046, 128067, 128053, 127758, 127468, 127167,
126799, 126761, 126469, 126431, 125885, 125830, 125827, 125401, 125006, 124634, 123820, 123703, 123421, 123182, 122264, 122201, 121603, 121276, 121207, 120739,
120733, 120733, 120590, 120186, 120145, 119807, 119753, 119666, 119506, 119114, 118860, 118852, 118834, 118754, 118641, 118443, 118250, 118114, 117921, 117386,
117251, 116955, 116467, 115905, 115788, 115195, 115006, 114054, 113807, 113611, 113578, 113357, 113352, 113104, 112921, 112889, 112447, 112285, 112125, 112044,
111849, 111678, 111194, 111114, 110932, 110898, 110834, 110754, 110549, 110500, 110314, 110296, 110250, 110061, 109894, 109837, 109662, 109449, 109330, 109160,
108983, 108781, 108701, 108580, 108544, 108419, 108299, 108140, 108097, 108072, 108034, 107787, 107496, 107475, 107414, 107167, 107012, 106996, 106992, 106981,
106858, 106661, 106591, 106445, 106415, 106184, 106108, 105775, 105765, 105588, 105412, 105403, 104938, 104606, 104448, 104326, 104205, 103981, 103755, 103497,
103468, 103346, 102885, 102872, 102830, 102801, 102355, 102206, 102124, 102039, 101875, 101771, 101717, 101569, 101139, 101117, 101061, 101037, 100710, 100695,
100522, 100504, 100499, 100292, 100235, 100166, 99803, 99654, 99562, 99439, 99305, 99221, 99158, 99074, 98995, 98872, 98774, 98756, 98715, 98705,
98651, 98498, 98233, 97628, 97567, 97353, 97334, 97094, 96908, 96835, 96681, 96548, 96465, 96414, 96198, 96183, 96170, 96110, 95980, 95538,
95524, 95398, 95391, 95371, 95303, 95150, 94978, 94833, 94707, 94578, 94480, 94379, 94284, 94124, 93840, 93675, 93470, 93398, 93385, 93235,
92801, 92446, 92292, 92260, 92217, 92204, 91996, 91985, 91969, 91938, 91592, 91393, 91306, 91208, 91116, 90924, 90632, 90514, 90412, 90386,
90352, 90319, 89756, 89723, 89650, 89563, 89151, 89125, 88382, 88208, 88164, 88134, 88005, 87879, 87876, 87794, 87573, 87538, 87446, 87340,
87300, 87235, 87074, 86745, 86702, 86560, 86479, 86272, 86257, 86241, 86158, 86132, 86122, 86020, 86008, 85992, 85875, 85874, 85833, 85778,
85761, 85701, 85694, 85488, 85454, 85391, 85325, 85257, 84796, 84789, 84784, 84750, 84637, 84636, 84584, 84518, 84500, 84365, 84323, 84298,
84154, 84072, 84004, 83908, 83854, 83814, 83716, 83640, 83486, 83482, 83422, 83249, 83151, 83147, 83124, 83075, 83039, 82918, 82883, 82840,
82831, 82726, 82644, 82624, 82567, 82562, 82339, 82223, 82127, 82118, 82035, 81975, 81962, 81900, 81890, 81804, 81671, 81480, 81458, 81397,
81393, 81232, 81109, 81056, 80757, 80651, 80463, 80427, 80295, 80294, 80239, 80239, 80070, 80046, 79876, 79860, 79820, 79753, 79751, 79636,
79606, 79482, 79432, 79425, 79419, 79376, 79367, 79302, 79280, 79179, 79163, 79110, 79086, 79053, 78859, 78810, 78788, 78564, 78318, 78306,
78282, 78246, 78174, 78040, 77716, 77368, 77300, 77287, 77261, 77250, 77220, 77184, 77028, 76977, 76974, 76923, 76672, 76597, 76593, 76560,
76548, 76541, 76496, 76402, 76333, 76068, 76060, 75991, 75918, 75874, 75860, 75856, 75850, 75814, 75473, 75430, 75422, 75405, 75348, 75254,
75210, 75153, 75151, 75128, 74981, 74907, 74834, 74746, 74439, 74413, 74392, 74378, 74343, 74293, 74266, 74220, 74211, 74196, 74053, 74036,
73914, 73906, 73882, 73833, 73826, 73755, 73727, 73622, 73582, 73527, 73443, 73438, 73339, 73328, 73314, 73302, 73281, 73267, 73233, 73182,
73178, 72926, 72890, 72815, 72644, 72571, 72550, 72545, 72509, 72450, 72405, 72333, 72268, 72253, 72161, 72098, 72034, 71894, 71888, 71885,
71878, 71849, 71766, 71752, 71744, 71689, 71655, 71583, 71559, 71399, 71196, 71190, 71072, 71042, 71041, 70949, 70862, 70764, 70650, 70644,
70641, 70610, 70525, 70513, 70453, 70414, 70265, 70228, 70213, 70067, 70018, 69973, 69861, 69854, 69772, 69761, 69731, 69666, 69664, 69635,
69628, 69603, 69602, 69594, 69533, 69508, 69443, 69415, 69368, 69288, 69280, 69212, 69116, 69073, 68996, 68932, 68905, 68882, 68874, 68830,
68603, 68506, 68472, 68394, 68234, 68193, 68050, 67943, 67880, 67815, 67762, 67660, 67596, 67590, 67551, 67492, 67424, 67318, 67277, 67259,
67229, 67152, 67088, 67064, 66998, 66981, 66946, 66940, 66828, 66803, 66499, 66476, 66429, 66263, 66262, 66220, 66141, 66066, 66055, 66004,
65994, 65960, 65884, 65819, 65708, 65645, 65640, 65616, 65581, 65564, 65554, 65397, 65284, 65255, 65118, 64982, 64972, 64837, 64720, 64706,
64659, 64642, 64593, 64404, 64398, 64364, 64311, 64271, 64227, 64155, 64055, 64005, 63988, 63988, 63905, 63861, 63855, 63852, 63794, 63775,
63775, 63768, 63717, 63712, 63709, 63695, 63578, 63558, 63536, 63526, 63492, 63480, 63423, 63420, 63341, 63313, 63253, 63179, 63163, 63155,
63125, 63091, 63068, 63064, 63013, 62980, 62951, 62942, 62895, 62856, 62831, 62815, 62808, 62771, 62740, 62727, 62650, 62645, 62587, 62536,
62480, 62460, 62316, 62288, 62148, 62138, 62120, 62114, 61966, 61954, 61954, 61920, 61893, 61786, 61504, 61385, 61378, 61338, 61326, 61314,
61248, 61220, 61160, 61097, 60999, 60988, 60983, 60982, 60967, 60926, 60897, 60858, 60837, 60770, 60753, 60648, 60553, 60553, 60526, 60526,
60476, 60436, 60402, 60385, 60340, 60299, 60227, 60210, 60200, 60085, 60068, 60045, 60002, 59979, 59970, 59925, 59874, 59843, 59832, 59635,
59632, 59569, 59565, 59511, 59480, 59413, 59406, 59325, 59262, 59181, 59172, 59171, 59160, 59052, 58980, 58941, 58940, 58899, 58893, 58841,
58811, 58806, 58806, 58789, 58761, 58711, 58700, 58670, 58632, 58557, 58552, 58499, 58458, 58379, 58370, 58269, 58218, 58196, 58189, 58148,
58140, 58106, 58088, 58020, 57960, 57959, 57947, 57773, 57721, 57715, 57661, 57519, 57510, 57496, 57471, 57458, 57449, 57390, 57389, 57341,
57326, 57296, 57292, 57284, 57251, 57225, 57212, 57199, 57188, 57115, 57041, 57038, 57035, 56981, 56835, 56833, 56807, 56755, 56749, 56737,
56699, 56630, 56561, 56556, 56527, 56510, 56455, 56114, 56095, 55894, 55850, 55849, 55848, 55827, 55810, 55776, 55772, 55718, 55667, 55667,
55657, 55626, 55610, 55604, 55599, 55588, 55576, 55568, 55551, 55453, 55432, 55401, 55328, 55324, 55307, 55265, 55228, 55209, 55194, 55186,
55162, 55162, 55123, 55075, 55022, 55000, 54946, 54939, 54923, 54921, 54920, 54836, 54817, 54796, 54773, 54765, 54731, 54720, 54628, 54559,
54525, 54471, 54426, 54408, 54381, 54364, 54344, 54334, 54173, 54173, 54169, 54149, 54148, 54054, 54031, 54020, 54010, 54009, 53931, 53915,
53905, 53846, 53834, 53784, 53776, 53769, 53754, 53742, 53699, 53674, 53589, 53552, 53542, 53527, 53503, 53489, 53424, 53413, 53386, 53168,
53158, 53114, 53096, 53061, 52958, 52920, 52902, 52897, 52870, 52846, 52841, 52711, 52703, 52678, 52677, 52618, 52615, 52606, 52581, 52519,
52512, 52512, 52482, 52449, 52407, 52379, 52322, 52313, 52292, 52212, 52201, 52123, 52106, 52101, 52050, 52050, 52000, 51981, 51951, 51940,
51907, 51889, 51833, 51818, 51799, 51777, 51713, 51674, 51669, 51594, 51591, 51547, 51540, 51456, 51428, 51396, 51394, 51380, 51315, 51307,
51298, 51285, 51211, 51205, 51201, 51077, 51056, 51050, 51009, 51007, 50988, 50978, 50906, 50726, 50691, 50676, 50674, 50651, 50620, 50605,
50588, 50552, 50465, 50461, 50447, 50436, 50335, 50258, 50246, 50201, 50185, 50168, 50042, 50000, 49955, 49929, 49903, 49899, 49887, 49841,
49820, 49810, 49804, 49719, 49716, 49655, 49642, 49629, 49580, 49579, 49574, 49572, 49550, 49545, 49520, 49489, 49481, 49428, 49424, 49391,
49369, 49339, 49305, 49262, 49226, 49174, 49171, 49161, 49125, 49102, 49074, 49070, 49003, 48962, 48948, 48839, 48810, 48809, 48752, 48702,
48636, 48619, 48614, 48561, 48529, 48503, 48495, 48476, 48473, 48413, 48411, 48405, 48402, 48372, 48331, 48262, 48239, 48224, 48202, 48198,
48062, 48060, 48021, 47984, 47962, 47956, 47924, 47921, 47849, 47824, 47818, 47778, 47738, 47699, 47674, 47671, 47620, 47619, 47609, 47599,
47589, 47574, 47550, 47530, 47519, 47490, 47488, 47392, 47390, 47333, 47326, 47324, 47317, 47268, 47254, 47194, 47190, 47129, 47110, 47095,
47084, 47063, 47056, 47052, 47022, 46902, 46878, 46854, 46847, 46838, 46811, 46797, 46769, 46752, 46747, 46717, 46691, 46647, 46622, 46561,
46561, 46465, 46447, 46408, 46386, 46383, 46370, 46299, 46288, 46271, 46254, 46252, 46240, 46194, 46145, 46112, 46109, 46094, 46085, 46054,
46045, 46024, 45996, 45988, 45918, 45800, 45788, 45758, 45744, 45735, 45733, 45697, 45696, 45690, 45680, 45638, 45581, 45568, 45558, 45524,
45463, 45458, 45448, 45439, 45424, 45376, 45310, 45281, 45281, 45270, 45265, 45251, 45248, 45241, 45216, 45182, 45124, 45123, 45107, 45100,
44958, 44944, 44925, 44877, 44856, 44826, 44822, 44818, 44813, 44675, 44660, 44629, 44626, 44568, 44551, 44487, 44474, 44455, 44454, 44441,
44412, 44412, 44402, 44398, 44368, 44354, 44316, 44293, 44279, 44260, 44256, 44254, 44248, 44208, 44175, 44136, 44128, 44098, 44067, 44053,
44022, 43999, 43985, 43982, 43978, 43966, 43960, 43950, 43950, 43943, 43919, 43893, 43885, 43847, 43838, 43838, 43784, 43682, 43611, 43610,
43606, 43603, 43561, 43534, 43530, 43504, 43495, 43432, 43406, 43305, 43234, 43178, 43177, 43144, 43081, 43072, 43056, 43044, 42938, 42903,
42899, 42889, 42863, 42830, 42802, 42768, 42699, 42697, 42695, 42658, 42656, 42649, 42641, 42635, 42614, 42602, 42597, 42596, 42559, 42481,
42462, 42435, 42347, 42330, 42272, 42255, 42245, 42234, 42224, 42216, 42208, 42204, 42189, 42152, 42102, 42082, 42076, 42072, 42018, 41993,
41987, 41921, 41912, 41884, 41826, 41750, 41746, 41729, 41728, 41725, 41705, 41692, 41618, 41500, 41490, 41480, 41432, 41349, 41339, 41335,
41312, 41304, 41288, 41281, 41278, 41232, 41204, 41195, 41186, 41145, 41118, 41106, 41104, 41101, 41099, 41080, 41073, 41030, 40983, 40936,
40926, 40860, 40843, 40827, 40818, 40811, 40788, 40773, 40752, 40713, 40691, 40691, 40670, 40661, 40658, 40639, 40614, 40597, 40546, 40523,
40488, 40484, 40484, 40479, 40478, 40476, 40449, 40414, 40383, 40380, 40372, 40351, 40348, 40315, 40278, 40262, 40222, 40214, 40201, 40181,
40170, 40165, 40161, 40160, 40157, 40157, 40128, 40118, 40089, 40080, 40061, 40014, 39957, 39933, 39916, 39912, 39909, 39901, 39872, 39859,
39848, 39846, 39843, 39842, 39831, 39813, 39808, 39774, 39758, 39723, 39713, 39698, 39660, 39660, 39592, 39587, 39576, 39566, 39561, 39534,
39520, 39479, 39463, 39426, 39411, 39402, 39398, 39324, 39316, 39278, 39258, 39251, 39202, 39198, 39181, 39108, 39093, 39085, 39051, 39048,
38991, 38990, 38967, 38956, 38949, 38941, 38883, 38876, 38860, 38837, 38811, 38686, 38668, 38651, 38633, 38630, 38576, 38558, 38550, 38536,
38535, 38527, 38525, 38510, 38505, 38474, 38463, 38424, 38402, 38361, 38356, 38353, 38322, 38304, 38301, 38284, 38240, 38202, 38187, 38186,
38164, 38156, 38152, 38142, 38131, 38110, 38039, 38038, 38034, 38021, 38004, 37997, 37992, 37966, 37923, 37914, 37879, 37833, 37816, 37796,
37788, 37771, 37764, 37740, 37736, 37732, 37704, 37701, 37689, 37674, 37667, 37665, 37659, 37642, 37623, 37611, 37594, 37580, 37542, 37529,
37515, 37462, 37393, 37376, 37338, 37330, 37325, 37315, 37274, 37264, 37259, 37242, 37220, 37215, 37209, 37162, 37159, 37142, 37133, 37132,
37127, 37114, 37113, 37107, 37090, 37052, 37041, 37036, 37032, 36986, 36974, 36917, 36888, 36880, 36873, 36778, 36775, 36767, 36747, 36725,
36699, 36654, 36643, 36635, 36629, 36564, 36562, 36516, 36513, 36482, 36471, 36451, 36440, 36414, 36383, 36382, 36356, 36337, 36312, 36288,
36285, 36268, 36256, 36256, 36239, 36238, 36221, 36182, 36158, 36156, 36156, 36123, 36080, 36007, 35997, 35978, 35959, 35958, 35918, 35896,
35881, 35854, 35849, 35843, 35830, 35810, 35808, 35805, 35777, 35777, 35725, 35704, 35671, 35632, 35618, 35542, 35520, 35509, 35506, 35494,
35492, 35449, 35448, 35384, 35353, 35311, 35281, 35281, 35277, 35270, 35240, 35233, 35181, 35178, 35152, 35122, 35100, 35091, 35089, 35058,
35025, 34986, 34966, 34954, 34932, 34924, 34921, 34911, 34854, 34847, 34842, 34842, 34842, 34792, 34791, 34762, 34748, 34700, 34626, 34619,
34617, 34593, 34549, 34546, 34523, 34514, 34512, 34501, 34484, 34455, 34419, 34395, 34363, 34350, 34347, 34329, 34328, 34290, 34282, 34277,
34224, 34164, 34154, 34145, 34144, 34144, 34137, 34114, 34107, 34068, 34064, 34042, 34024, 34000, 33989, 33961, 33959, 33948, 33937, 33915,
33876, 33808, 33788, 33780, 33722, 33701, 33693, 33687, 33684, 33679, 33648, 33590, 33588, 33576, 33479, 33465, 33464, 33455, 33435, 33396,
33394, 33392, 33383, 33375, 33362, 33290, 33276, 33218, 33216, 33203, 33203, 33168, 33148, 33135, 33108, 33084, 33080, 33060, 33060, 33034,
33032, 33008, 33000, 32952, 32925, 32920, 32902, 32859, 32835, 32809, 32782, 32753, 32732, 32720, 32714, 32668, 32667, 32665, 32617, 32603,
32603, 32587, 32565, 32520, 32509, 32503, 32488, 32473, 32456, 32451, 32405, 32402, 32388, 32342, 32325, 32285, 32285, 32278, 32248, 32243,
32240, 32199, 32182, 32178, 32134, 32114, 32052, 32050, 32038, 32031, 32030, 32023, 32009, 31995, 31990, 31980, 31969, 31949, 31935, 31921,
31848, 31834, 31804, 31802, 31786, 31775, 31770, 31752, 31743, 31736, 31727, 31725, 31701, 31696, 31689, 31689, 31679, 31661, 31633, 31622,
31607, 31587, 31569, 31554, 31550, 31523, 31518, 31490, 31473, 31473, 31472, 31465, 31391, 31383, 31357, 31352, 31348, 31347, 31343, 31335,
31324, 31322, 31291, 31268, 31254, 31233, 31212, 31210, 31185, 31182, 31166, 31144, 31135, 31132, 31105, 31103, 31094, 31085, 31056, 31006,
31001, 30976, 30971, 30966, 30933, 30925, 30910, 30891, 30886, 30872, 30872, 30843, 30835, 30804, 30800, 30799, 30751, 30738, 30706, 30700,
30695, 30693, 30679, 30651, 30644, 30626, 30622, 30600, 30546, 30538, 30526, 30488, 30486, 30483, 30482, 30451, 30448, 30432, 30400, 30399,
30389, 30323, 30312, 30296, 30294, 30292, 30288, 30249, 30244, 30239, 30234, 30228, 30202, 30186, 30184, 30177, 30166, 30126, 30121, 30110,
30107, 30103, 30093, 30028, 30020, 30011, 30000, 29993, 29987, 29984, 29965, 29931, 29915, 29906, 29901, 29891, 29884, 29863, 29855, 29830,
29817, 29813, 29807, 29803, 29792, 29792, 29758, 29755, 29747, 29747, 29742, 29705, 29704, 29681, 29679, 29668, 29662, 29642, 29599, 29568,
29544, 29535, 29470, 29453, 29452, 29445, 29445, 29406, 29406, 29381, 29369, 29363, 29359, 29346, 29346, 29334, 29331, 29324, 29319, 29317,
29302, 29288, 29285, 29273, 29260, 29254, 29188, 29179, 29158, 29122, 29103, 29095, 29094, 29086, 29054, 29046, 29037, 29023, 29014, 29002,
28978, 28974, 28972, 28942, 28924, 28919, 28880, 28853, 28841, 28834, 28821, 28805, 28803, 28802, 28796, 28794, 28790, 28741, 28733, 28730,
28722, 28720, 28717, 28690, 28684, 28679, 28679, 28660, 28623, 28608, 28584, 28565, 28561, 28547, 28545, 28531, 28528, 28505, 28491, 28478,
28468, 28451, 28451, 28433, 28407, 28369, 28357, 28353, 28346, 28324, 28321, 28297, 28296, 28267, 28240, 28238, 28227, 28214, 28207, 28162,
28149, 28147, 28146, 28137, 28137, 28132, 28121, 28114, 28110, 28107, 28106, 28103, 28102, 28083, 28082, 28073, 28064, 28043, 28028, 28020,
28019, 28017, 27970, 27952, 27931, 27910, 27886, 27884, 27879, 27874, 27867, 27862, 27855, 27851, 27839, 27830, 27826, 27819, 27814, 27812,
27778, 27776, 27766, 27766, 27766, 27751, 27738, 27713, 27694, 27690, 27679, 27666, 27662, 27660, 27637, 27624, 27611, 27586, 27580, 27546,
27544, 27535, 27506, 27504, 27496, 27485, 27484, 27457, 27452, 27445, 27404, 27403, 27385, 27380, 27351, 27346, 27340, 27321, 27321, 27302,
27296, 27285, 27277, 27262, 27258, 27249, 27239, 27230, 27221, 27217, 27210, 27209, 27204, 27202, 27202, 27199, 27191, 27189, 27175, 27163,
27160, 27160, 27141, 27133, 27110, 27100, 27081, 27076, 27065, 27038, 27038, 27038, 27035, 27018, 26997, 26961, 26960, 26960, 26916, 26909,
26902, 26898, 26858, 26838, 26834, 26833, 26816, 26789, 26789, 26783, 26781, 26767, 26758, 26754, 26748, 26728, 26716, 26696, 26694, 26690,
26650, 26627, 26606, 26603, 26592, 26583, 26574, 26556, 26542, 26535, 26519, 26513, 26512, 26508, 26504, 26489, 26481, 26479, 26478, 26469,
26448, 26403, 26381, 26376, 26362, 26353, 26350, 26346, 26344, 26342, 26342, 26319, 26307, 26300, 26284, 26270, 26240, 26230, 26210, 26204,
26202, 26199, 26184, 26184, 26177, 26159, 26152, 26145, 26143, 26140, 26129, 26113, 26093, 26092, 26089, 26070, 26043, 26041, 26037, 26024,
26023, 26013, 26013, 25980, 25971, 25969, 25963, 25926, 25915, 25914, 25912, 25910, 25906, 25902, 25876, 25871, 25864, 25844, 25826, 25823,
25785, 25779, 25774, 25770, 25769, 25766, 25764, 25759, 25738, 25719, 25715, 25714, 25710, 25668, 25668, 25642, 25616, 25582, 25580, 25533,
25512, 25422, 25417, 25411, 25410, 25408, 25394, 25367, 25364, 25339, 25338, 25314, 25308, 25290, 25289, 25284, 25279, 25251, 25213, 25210,
25204, 25170, 25166, 25156, 25153, 25150, 25120, 25107, 25103, 25071, 25046, 25028, 25010, 25009, 24996, 24989, 24976, 24963, 24960, 24958,
24956, 24952, 24952, 24950, 24933, 24925, 24922, 24907, 24888, 24879, 24870, 24846, 24818, 24805, 24803, 24790, 24776, 24755, 24752, 24726,
24721, 24717, 24711, 24702, 24693, 24672, 24659, 24654, 24653, 24634, 24613, 24600, 24596, 24594, 24579, 24576, 24575, 24562, 24554, 24552,
24534, 24516, 24510, 24508, 24482, 24476, 24467, 24465, 24455, 24438, 24387, 24372, 24367, 24367, 24365, 24350, 24343, 24340, 24338, 24324,
24297, 24296, 24270, 24263, 24262, 24259, 24249, 24244, 24242, 24215, 24198, 24197, 24190, 24182, 24179, 24176, 24171, 24165, 24146, 24111,
24106, 24103, 24100, 24080, 24066, 24050, 24039, 24036, 24034, 24031, 24030, 24016, 24014, 23981, 23956, 23953, 23951, 23920, 23918, 23902,
23900, 23868, 23866, 23826, 23785, 23780, 23775, 23757, 23749, 23733, 23711, 23705, 23702, 23694, 23689, 23680, 23669, 23665, 23636, 23629,
23620, 23617, 23614, 23610, 23609, 23571, 23570, 23562, 23557, 23554, 23554, 23550, 23542, 23541, 23535, 23529, 23524, 23524, 23516, 23511,
23508, 23506, 23494, 23493, 23490, 23483, 23477, 23466, 23447, 23436, 23422, 23376, 23368, 23365, 23364, 23335, 23335, 23327, 23320, 23304,
23303, 23300, 23295, 23259, 23246, 23241, 23229, 23228, 23220, 23216, 23216, 23213, 23209, 23206, 23168, 23160, 23152, 23150, 23135, 23124,
23122, 23114, 23100, 23094, 23094, 23090, 23082, 23071, 23042, 23040, 23033, 23020, 23020, 23019, 22993, 22992, 22986, 22982, 22982, 22936,
22933, 22922, 22903, 22902, 22900, 22845, 22845, 22842, 22823, 22808, 22789, 22781, 22770, 22753, 22749, 22749, 22741, 22736, 22736, 22727,
22696, 22691, 22683, 22673, 22658, 22648, 22647, 22640, 22638, 22628, 22592, 22587, 22586, 22584, 22575, 22574, 22549, 22547, 22539, 22526,
22523, 22520, 22519, 22502, 22500, 22497, 22488, 22482, 22481, 22466, 22455, 22446, 22420, 22419, 22416, 22410, 22410, 22402, 22396, 22375,
22364, 22364, 22362, 22355, 22346, 22346, 22334, 22329, 22328, 22327, 22305, 22299, 22289, 22286, 22282, 22272, 22261, 22255, 22252, 22247,
22245, 22228, 22222, 22209, 22207, 22198, 22175, 22171, 22167, 22162, 22160, 22153, 22149, 22122, 22121, 22120, 22089, 22088, 22082, 22075,
22065, 22059, 22058, 22057, 22052, 22052, 22033, 22015, 22012, 21975, 21948, 21929, 21926, 21922, 21915, 21910, 21895, 21881, 21868, 21857,
21851, 21847, 21837, 21837, 21827, 21813, 21804, 21804, 21804, 21803, 21798, 21777, 21770, 21762, 21755, 21750, 21748, 21741, 21736, 21729,
21718, 21717, 21713, 21707, 21695, 21661, 21660, 21649, 21632, 21622, 21615, 21607, 21576, 21573, 21573, 21562, 21552, 21520, 21516, 21516,
21512, 21498, 21497, 21496, 21433, 21432, 21428, 21414, 21412, 21410, 21404, 21403, 21399, 21398, 21391, 21381, 21379, 21377, 21374, 21361,
21357, 21353, 21349, 21347, 21339, 21334, 21330, 21327, 21323, 21309, 21307, 21300, 21293, 21292, 21282, 21276, 21269, 21265, 21264, 21244,
21239, 21222, 21201, 21198, 21196, 21194, 21178, 21178, 21173, 21166, 21156, 21154, 21142, 21137, 21136, 21134, 21130, 21128, 21126, 21125,
21122, 21121, 21119, 21098, 21091, 21090, 21087, 21084, 21084, 21073, 21071, 21067, 21053, 21050, 21046, 21028, 21028, 21022, 21015, 21010,
21003, 20999, 20994, 20982, 20973, 20964, 20950, 20948, 20945, 20943, 20939, 20934, 20898, 20895, 20895, 20895, 20883, 20870, 20866, 20864,
20848, 20843, 20819, 20815, 20813, 20804, 20802, 20796, 20784, 20774, 20756, 20754, 20752, 20748, 20735, 20723, 20717, 20715, 20710, 20702,
20700, 20700, 20698, 20696, 20695, 20692, 20679, 20650, 20649, 20634, 20629, 20629, 20622, 20614, 20614, 20603, 20600, 20588, 20588, 20577,
20573, 20559, 20549, 20496, 20495, 20494, 20493, 20491, 20489, 20487, 20485, 20479, 20448, 20441, 20435, 20431, 20424, 20419, 20419, 20410,
20406, 20381, 20362, 20357, 20354, 20342, 20328, 20316, 20303, 20247, 20244, 20229, 20220, 20217, 20213, 20213, 20213, 20210, 20209, 20197,
20193, 20191, 20190, 20187, 20186, 20176, 20161, 20159, 20152, 20146, 20124, 20119, 20107, 20085, 20077, 20072, 20054, 20048, 20043, 20040,
20032, 20031, 20024, 20017, 20016, 20006, 20005, 20002, 19999, 19977, 19971, 19953, 19917, 19901, 19901, 19892, 19884, 19879, 19873, 19868,
19867, 19867, 19866, 19858, 19854, 19848, 19815, 19793, 19771, 19770, 19767, 19751, 19736, 19723, 19722, 19721, 19714, 19713, 19694, 19690,
19690, 19687, 19681, 19656, 19649, 19647, 19639, 19638, 19629, 19593, 19564, 19547, 19541, 19526, 19508, 19497, 19486, 19478, 19452, 19452,
19426, 19418, 19406, 19399, 19376, 19375, 19372, 19372, 19369, 19347, 19338, 19338, 19335, 19332, 19327, 19324, 19316, 19314, 19311, 19308,
19294, 19291, 19279, 19278, 19277, 19251, 19246, 19236, 19228, 19227, 19226, 19221, 19216, 19215, 19212, 19210, 19206, 19192, 19173, 19168,
19161, 19157, 19149, 19147, 19143, 19142, 19138, 19138, 19136, 19130, 19122, 19118, 19117, 19111, 19110, 19100, 19096, 19093, 19090, 19088,
19088, 19069, 19062, 19061, 19042, 19030, 19024, 19020, 19005, 19004, 18992, 18971, 18965, 18963, 18961, 18940, 18937, 18927, 18921, 18909,
18905, 18898, 18892, 18873, 18858, 18857, 18856, 18849, 18848, 18848, 18838, 18837, 18834, 18829, 18824, 18823, 18820, 18816, 18815, 18810,
18806, 18755, 18753, 18748, 18735, 18724, 18723, 18721, 18708, 18702, 18701, 18688, 18671, 18670, 18661, 18660, 18660, 18652, 18642, 18634,
18632, 18626, 18615, 18599, 18597, 18595, 18587, 18586, 18583, 18576, 18573, 18570, 18569, 18567, 18565, 18564, 18564, 18559, 18549, 18546,
18541, 18541, 18530, 18528, 18526, 18508, 18504, 18498, 18494, 18478, 18469, 18463, 18457, 18454, 18441, 18431, 18428, 18398, 18392, 18391,
18389, 18389, 18383, 18381, 18376, 18366, 18344, 18339, 18336, 18328, 18326, 18324, 18324, 18321, 18318, 18312, 18310, 18307, 18300, 18296,
18292, 18276, 18274, 18271, 18265, 18254, 18252, 18248, 18243, 18232, 18216, 18207, 18193, 18187, 18177, 18168, 18164, 18160, 18153, 18152,
18145, 18144, 18110, 18096, 18084, 18079, 18076, 18074, 18074, 18073, 18073, 18068, 18066, 18063, 18058, 18058, 18054, 18053, 18052, 18049,
18039, 18026, 18021, 18018, 18008, 18004, 18004, 18001, 17999, 17998, 17995, 17994, 17992, 17990, 17985, 17984, 17972, 17960, 17955, 17954,
17952, 17933, 17933, 17918, 17913, 17910, 17906, 17906, 17904, 17903, 17898, 17896, 17893, 17892, 17887, 17876, 17859, 17857, 17857, 17854,
17848, 17823, 17820, 17817, 17814, 17805, 17793, 17783, 17774, 17773, 17769, 17758, 17753, 17748, 17746, 17744, 17740, 17737, 17730, 17725,
17724, 17716, 17715, 17714, 17713, 17710, 17698, 17678, 17667, 17655, 17655, 17654, 17640, 17637, 17631, 17630, 17628, 17615, 17612, 17608,
17606, 17604, 17600, 17584, 17577, 17560, 17553, 17544, 17543, 17541, 17539, 17537, 17527, 17526, 17521, 17520, 17519, 17507, 17501, 17496,
17494, 17472, 17467, 17466, 17464, 17464, 17464, 17460, 17453, 17452, 17440, 17437, 17429, 17424, 17421, 17416, 17415, 17401, 17394, 17393,
17392, 17392, 17373, 17358, 17354, 17343, 17342, 17329, 17322, 17315, 17308, 17302, 17295, 17290, 17288, 17287, 17285, 17281, 17278, 17277,
17276, 17268, 17262, 17261, 17248, 17246, 17245, 17240, 17235, 17233, 17233, 17230, 17229, 17228, 17226, 17221, 17210, 17210, 17193, 17170,
17167, 17164, 17164, 17163, 17161, 17161, 17141, 17127, 17126, 17126, 17122, 17114, 17114, 17112, 17112, 17111, 17104, 17096, 17090, 17087,
17080, 17076, 17066, 17063, 17060, 17057, 17038, 17038, 17036, 17032, 17032, 17029, 17016, 17009, 17006, 17005, 17004, 16999, 16995, 16994,
16993, 16986, 16985, 16984, 16983, 16971, 16955, 16951, 16944, 16940, 16937, 16933, 16931, 16929, 16926, 16926, 16918, 16914, 16914, 16903,
16902, 16892, 16886, 16872, 16869, 16865, 16863, 16859, 16858, 16850, 16849, 16843, 16834, 16834, 16834, 16832, 16826, 16816, 16813, 16812,
16802, 16801, 16800, 16792, 16792, 16790, 16786, 16786, 16784, 16783, 16779, 16777, 16768, 16766, 16766, 16757, 16756, 16753, 16749, 16743,
16740, 16735, 16734, 16721, 16721, 16689, 16683, 16682, 16645, 16645, 16642, 16635, 16634, 16631, 16616, 16611, 16607, 16592, 16590, 16575,
16571, 16566, 16564, 16561, 16558, 16556, 16552, 16547, 16545, 16532, 16530, 16522, 16520, 16501, 16496, 16494, 16475, 16473, 16471, 16467,
16456, 16451, 16436, 16432, 16428, 16427, 16426, 16418, 16411, 16408, 16402, 16399, 16392, 16391, 16383, 16383, 16376, 16363, 16357, 16354,
16349, 16343, 16333, 16327, 16326, 16322, 16320, 16318, 16317, 16311, 16281, 16266, 16261, 16255, 16254, 16252, 16249, 16248, 16248, 16244,
16243, 16237, 16236, 16222, 16220, 16215, 16204, 16200, 16198, 16170, 16152, 16148, 16145, 16145, 16130, 16126, 16124, 16106, 16095, 16095,
16088, 16081, 16077, 16076, 16072, 16054, 16054, 16048, 16044, 16043, 16036, 16028, 16028, 16021, 16019, 16018, 16016, 16005, 16001, 15999,
15994, 15984, 15981, 15978, 15973, 15972, 15971, 15955, 15954, 15945, 15944, 15936, 15931, 15914, 15912, 15910, 15910, 15904, 15895, 15895,
15894, 15885, 15885, 15884, 15882, 15873, 15864, 15859, 15854, 15850, 15846, 15846, 15834, 15823, 15823, 15819, 15815, 15802, 15786, 15785,
15781, 15778, 15769, 15765, 15761, 15760, 15755, 15748, 15738, 15736, 15732, 15732, 15722, 15721, 15717, 15706, 15702, 15696, 15694, 15692,
15677, 15675, 15670, 15668, 15668, 15666, 15665, 15665, 15656, 15645, 15642, 15634, 15632, 15630, 15626, 15615, 15610, 15608, 15601, 15597,
15569, 15568, 15563, 15560, 15558, 15558, 15545, 15533, 15524, 15523, 15510, 15497, 15493, 15492, 15482, 15474, 15471, 15471, 15468, 15467,
15465, 15462, 15460, 15448, 15448, 15448, 15446, 15446, 15430, 15429, 15429, 15427, 15426, 15426, 15421, 15417, 15409, 15402, 15402, 15393,
15389, 15385, 15380, 15374, 15372, 15365, 15353, 15338, 15330, 15330, 15325, 15321, 15319, 15316, 15304, 15302, 15290, 15289, 15286, 15285,
15281, 15272, 15271, 15253, 15252, 15248, 15247, 15240, 15236, 15233, 15228, 15225, 15223, 15221, 15217, 15217, 15213, 15208, 15202, 15202,
15191, 15187, 15172, 15172, 15168, 15167, 15162, 15160, 15159, 15157, 15157, 15152, 15152, 15142, 15140, 15138, 15130, 15123, 15110, 15108,
15092, 15091, 15087, 15085, 15067, 15066, 15061, 15050, 15046, 15042, 15032, 15021, 15020, 15016, 15011, 15009, 15008, 15005, 14994, 14994,
14994, 14979, 14975, 14974, 14959, 14954, 14948, 14945, 14940, 14939, 14935, 14934, 14933, 14923, 14918, 14913, 14905, 14894, 14892, 14884,
14883, 14883, 14879, 14879, 14876, 14872, 14871, 14868, 14866, 14846, 14840, 14840, 14836, 14831, 14831, 14829, 14822, 14822, 14814, 14803,
14801, 14795, 14786, 14783, 14780, 14772, 14759, 14756, 14754, 14751, 14749, 14748, 14744, 14738, 14734, 14732, 14727, 14726, 14708, 14706,
14706, 14703, 14702, 14690, 14689, 14687, 14683, 14683, 14675, 14668, 14658, 14657, 14653, 14649, 14645, 14639, 14637, 14624, 14621, 14619,
14618, 14617, 14617, 14615, 14614, 14611, 14602, 14599, 14595, 14578, 14576, 14572, 14569, 14566, 14560, 14555, 14554, 14548, 14546, 14540,
14538, 14534, 14518, 14517, 14517, 14510, 14505, 14503, 14502, 14495, 14492, 14489, 14474, 14471, 14468, 14464, 14461, 14461, 14460, 14460,
14456, 14454, 14448, 14447, 14433, 14432, 14429, 14418, 14416, 14410, 14408, 14406, 14403, 14394, 14392, 14391, 14388, 14384, 14376, 14370,
14367, 14365, 14360, 14360, 14358, 14357, 14356, 14351, 14342, 14342, 14341, 14335, 14330, 14326, 14324, 14314, 14314, 14309, 14303, 14296,
14289, 14284, 14278, 14270, 14270, 14264, 14262, 14258, 14257, 14254, 14252, 14248, 14247, 14237, 14236, 14234, 14226, 14225, 14222, 14210,
14208, 14207, 14206, 14206, 14200, 14193, 14192, 14188, 14184, 14180, 14176, 14174, 14161, 14156, 14142, 14141, 14139, 14137, 14135, 14126,
14122, 14122, 14122, 14118, 14115, 14114, 14110, 14108, 14106, 14106, 14101, 14096, 14088, 14072, 14072, 14064, 14062, 14055, 14042, 14032,
14028, 14013, 14009, 14001, 13997, 13992, 13989, 13977, 13970, 13967, 13965, 13964, 13960, 13958, 13956, 13948, 13948, 13944, 13933, 13924,
13920, 13918, 13918, 13914, 13910, 13910, 13903, 13899, 13885, 13884, 13880, 13876, 13869, 13866, 13857, 13856, 13853, 13853, 13851, 13850,
13850, 13846, 13840, 13837, 13831, 13818, 13808, 13805, 13803, 13800, 13796, 13785, 13784, 13779, 13776, 13773, 13772, 13764, 13763, 13761,
13758, 13755, 13752, 13748, 13744, 13743, 13740, 13738, 13734, 13734, 13732, 13731, 13729, 13725, 13724, 13723, 13721, 13718, 13712, 13712,
13707, 13703, 13700, 13697, 13688, 13687, 13684, 13682, 13680, 13676, 13670, 13670, 13669, 13666, 13663, 13661, 13656, 13655, 13651, 13649,
13647, 13642, 13642, 13640, 13616, 13612, 13609, 13604, 13603, 13601, 13596, 13592, 13576, 13568, 13567, 13555, 13554, 13543, 13539, 13536,
13534, 13533, 13520, 13519, 13516, 13516, 13512, 13511, 13511, 13509, 13507, 13497, 13492, 13482, 13478, 13478, 13477, 13468, 13463, 13460,
13456, 13454, 13452, 13438, 13436, 13436, 13434, 13433, 13421, 13418, 13411, 13407, 13406, 13403, 13402, 13393, 13384, 13377, 13376, 13368,
13358, 13356, 13346, 13345, 13341, 13334, 13331, 13323, 13321, 13321, 13317, 13303, 13292, 13291, 13289, 13287, 13286, 13286, 13280, 13279,
13278, 13274, 13269, 13267, 13267, 13259, 13257, 13247, 13246, 13242, 13241, 13236, 13236, 13236, 13235, 13233, 13232, 13229, 13228, 13227,
13218, 13217, 13205, 13203, 13202, 13190, 13185, 13183, 13180, 13176, 13172, 13172, 13162, 13159, 13154, 13151, 13147, 13145, 13145, 13142,
13140, 13139, 13129, 13128, 13121, 13119, 13117, 13117, 13114, 13105, 13104, 13104, 13096, 13092, 13078, 13076, 13066, 13066, 13066, 13064,
13061, 13054, 13053, 13050, 13045, 13045, 13043, 13043, 13041, 13041, 13040, 13025, 13024, 13024, 13019, 13011, 13009, 13008, 13006, 13002,
12999, 12993, 12990, 12988, 12987, 12982, 12980, 12978, 12976, 12971, 12966, 12960, 12960, 12959, 12958, 12957, 12956, 12956, 12952, 12952,
12950, 12950, 12948, 12942, 12936, 12920, 12919, 12917, 12913, 12910, 12904, 12903, 12892, 12890, 12872, 12869, 12864, 12856, 12855, 12851,
12848, 12846, 12840, 12838, 12830, 12827, 12818, 12814, 12812, 12809, 12805, 12803, 12793, 12792, 12786, 12782, 12779, 12775, 12760, 12758,
12756, 12755, 12754, 12752, 12745, 12744, 12739, 12736, 12736, 12735, 12731, 12723, 12717, 12714, 12710, 12709, 12706, 12693, 12692, 12691,
12688, 12685, 12672, 12668, 12662, 12656, 12655, 12653, 12653, 12643, 12642, 12638, 12638, 12631, 12630, 12620, 12618, 12617, 12616, 12612,
12610, 12607, 12600, 12600, 12600, 12600, 12591, 12589, 12588, 12584, 12583, 12577, 12577, 12574, 12569, 12562, 12562, 12560, 12559, 12556,
12541, 12538, 12536, 12530, 12530, 12528, 12528, 12520, 12518, 12513, 12510, 12509, 12509, 12502, 12498, 12497, 12495, 12494, 12487, 12482,
12482, 12478, 12477, 12471, 12467, 12466, 12456, 12456, 12446, 12444, 12444, 12435, 12433, 12428, 12418, 12412, 12399, 12398, 12395, 12395,
12394, 12394, 12392, 12391, 12386, 12385, 12384, 12381, 12379, 12376, 12375, 12373, 12358, 12357, 12355, 12346, 12341, 12334, 12333, 12326,
12325, 12318, 12312, 12308, 12304, 12303, 12303, 12293, 12293, 12292, 12285, 12285, 12276, 12274, 12272, 12252, 12251, 12244, 12244, 12244,
12241, 12239, 12237, 12236, 12233, 12227, 12227, 12217, 12214, 12204, 12204, 12202, 12202, 12194, 12194, 12194, 12188, 12187, 12186, 12183,
12183, 12179, 12178, 12176, 12175, 12174, 12171, 12165, 12164, 12164, 12159, 12149, 12146, 12143, 12137, 12135, 12131, 12129, 12127, 12123,
12121, 12116, 12115, 12108, 12102, 12100, 12100, 12096, 12096, 12095, 12094, 12092, 12090, 12090, 12087, 12086, 12084, 12081, 12079, 12075,
12072, 12066, 12063, 12054, 12050, 12049, 12048, 12037, 12036, 12028, 12026, 12026, 12024, 12017, 12012, 12010, 12009, 12006, 12001, 12000,
12000, 11996, 11996, 11994, 11990, 11989, 11984, 11982, 11977, 11976, 11975, 11972, 11972, 11965, 11964, 11957, 11956, 11952, 11937, 11930,
11930, 11927, 11927, 11922, 11917, 11911, 11906, 11901, 11901, 11896, 11891, 11889, 11881, 11880, 11873, 11869, 11868, 11865, 11861, 11858,
11856, 11855, 11851, 11850, 11847, 11845, 11845, 11838, 11836, 11835, 11831, 11830, 11828, 11824, 11818, 11816, 11815, 11812, 11806, 11800,
11799, 11798, 11797, 11795, 11793, 11793, 11789, 11786, 11782, 11778, 11776, 11775, 11772, 11771, 11762, 11755, 11754, 11754, 11752, 11752,
11751, 11747, 11745, 11740, 11732, 11730, 11728, 11725, 11720, 11718, 11718, 11718, 11716, 11714, 11711, 11711, 11710, 11702, 11698, 11695,
11693, 11691, 11689, 11682, 11679, 11679, 11669, 11668, 11667, 11666, 11666, 11654, 11648, 11647, 11647, 11639, 11636, 11633, 11633, 11629,
11620, 11616, 11612, 11612, 11609, 11604, 11604, 11600, 11586, 11582, 11579, 11576, 11574, 11570, 11569, 11568, 11561, 11551, 11550, 11550,
11545, 11543, 11542, 11542, 11538, 11537, 11528, 11524, 11523, 11522, 11516, 11508, 11508, 11503, 11502, 11497, 11494, 11493, 11493, 11488,
11482, 11478, 11477, 11462, 11447, 11444, 11444, 11443, 11442, 11440, 11430, 11427, 11426, 11426, 11422, 11418, 11418, 11414, 11403, 11402,
11398, 11398, 11397, 11394, 11392, 11385, 11380, 11380, 11376, 11370, 11365, 11361, 11358, 11358, 11358, 11354, 11346, 11343, 11331, 11330,
11330, 11327, 11326, 11316, 11313, 11309, 11309, 11302, 11301, 11300, 11293, 11290, 11283, 11274, 11274, 11266, 11266, 11248, 11247, 11243,
11242, 11241, 11240, 11240, 11239, 11238, 11237, 11232, 11230, 11225, 11225, 11222, 11221, 11217, 11216, 11215, 11214, 11208, 11206, 11206,
11204, 11204, 11198, 11198, 11193, 11183, 11180, 11176, 11176, 11173, 11172, 11172, 11170, 11169, 11169, 11167, 11166, 11166, 11159, 11158,
11156, 11155, 11153, 11147, 11146, 11146, 11146, 11142, 11138, 11137, 11134, 11128, 11127, 11123, 11111, 11111, 11111, 11105, 11104, 11100,
11097, 11096, 11082, 11077, 11074, 11074, 11067, 11062, 11062, 11058, 11050, 11049, 11048, 11047, 11046, 11045, 11042, 11041, 11033, 11022,
11016, 11008, 11006, 11005, 10997, 10992, 10986, 10985, 10985, 10984, 10982, 10979, 10977, 10976, 10974, 10973, 10971, 10971, 10967, 10948,
10947, 10944, 10944, 10942, 10935, 10931, 10928, 10922, 10922, 10922, 10906, 10901, 10899, 10892, 10891, 10888, 10887, 10884, 10881, 10880,
10878, 10875, 10869, 10865, 10857, 10851, 10850, 10848, 10846, 10836, 10836, 10834, 10827, 10824, 10820, 10820, 10819, 10818, 10816, 10812,
10809, 10804, 10797, 10797, 10797, 10795, 10789, 10785, 10783, 10778, 10777, 10761, 10760, 10759, 10755, 10752, 10743, 10740, 10740, 10738,
10736, 10736, 10729, 10724, 10721, 10720, 10715, 10715, 10711, 10711, 10710, 10705, 10704, 10699, 10698, 10698, 10697, 10694, 10692, 10689,
10687, 10683, 10681, 10681, 10678, 10671, 10661, 10660, 10660, 10658, 10656, 10656, 10653, 10649, 10647, 10646, 10646, 10643, 10639, 10634,
10634, 10632, 10632, 10626, 10621, 10615, 10612, 10609, 10602, 10598, 10598, 10598, 10597, 10594, 10592, 10592, 10591, 10587, 10586, 10584,
10582, 10581, 10576, 10573, 10571, 10571, 10570, 10568, 10566, 10558, 10556, 10551, 10551, 10542, 10541, 10539, 10535, 10532, 10531, 10531,
10530, 10530, 10525, 10525, 10523, 10512, 10510, 10510, 10507, 10502, 10501, 10501, 10495, 10494, 10494, 10492, 10490, 10489, 10486, 10482,
10482, 10480, 10478, 10476, 10476, 10475, 10475, 10473, 10471, 10466, 10466, 10464, 10459, 10456, 10452, 10443, 10441, 10440, 10432, 10427,
10426, 10424, 10422, 10418, 10416, 10408, 10407, 10406, 10400, 10396, 10391, 10382, 10380, 10378, 10374, 10372, 10370, 10368, 10365, 10355,
10352, 10352, 10351, 10350, 10348, 10346, 10343, 10334, 10332, 10331, 10330, 10324, 10323, 10321, 10320, 10320, 10316, 10314, 10314, 10311,
10308, 10308, 10303, 10299, 10296, 10296, 10293, 10291, 10290, 10289, 10284, 10274, 10274, 10269, 10267, 10266, 10266, 10266, 10262, 10261,
10260, 10259, 10259, 10252, 10251, 10245, 10245, 10245, 10240, 10229, 10222, 10217, 10217, 10214, 10210, 10208, 10199, 10195, 10195, 10191,
10187, 10182, 10180, 10180, 10180, 10179, 10177, 10172, 10169, 10168, 10162, 10159, 10153, 10151, 10145, 10145, 10144, 10142, 10141, 10138,
10138, 10137, 10136, 10135, 10133, 10122, 10120, 10119, 10116, 10112, 10111, 10109, 10107, 10107, 10105, 10102, 10099, 10098, 10093, 10092,
10090, 10088, 10085, 10074, 10056, 10054, 10047, 10046, 10043, 10041, 10036, 10034, 10030, 10030, 10027, 10024, 10021, 10016, 10016, 10015,
10009, 10005, 10005, 10002, 10000, 9996, 9994, 9994, 9987, 9986, 9984, 9979, 9975, 9971, 9971, 9971, 9970, 9969, 9968, 9964,
9964, 9962, 9960, 9958, 9956, 9956, 9954, 9950, 9949, 9948, 9941, 9934, 9928, 9926, 9921, 9914, 9911, 9908, 9905, 9905,
9904, 9903, 9902, 9898, 9891, 9889, 9884, 9876, 9873, 9871, 9866, 9866, 9864, 9859, 9855, 9854, 9852, 9851, 9851, 9850,
9849, 9843, 9839, 9839, 9836, 9835, 9835, 9833, 9832, 9830, 9829, 9827, 9825, 9820, 9817, 9816, 9816, 9812, 9808, 9808,
9807, 9806, 9805, 9803, 9797, 9796, 9793, 9788, 9787, 9782, 9781, 9780, 9777, 9772, 9772, 9771, 9767, 9755, 9752, 9752,
9751, 9750, 9749, 9748, 9748, 9748, 9746, 9745, 9743, 9740, 9736, 9736, 9735, 9735, 9734, 9731, 9729, 9728, 9725, 9722,
9721, 9721, 9719, 9716, 9715, 9706, 9700, 9700, 9699, 9697, 9695, 9687, 9684, 9684, 9683, 9680, 9678, 9676, 9675, 9673,
9672, 9672, 9669, 9661, 9659, 9655, 9653, 9650, 9649, 9649, 9648, 9647, 9647, 9647, 9642, 9642, 9639, 9638, 9637, 9637,
9635, 9632, 9631, 9628, 9627, 9627, 9626, 9626, 9624, 9623, 9616, 9615, 9612, 9611, 9596, 9592, 9590, 9589, 9589, 9589,
9586, 9581, 9579, 9576, 9576, 9574, 9574, 9572, 9570, 9568, 9568, 9567, 9566, 9562, 9559, 9553, 9547, 9543, 9540, 9534,
9532, 9526, 9524, 9523, 9519, 9517, 9515, 9513, 9511, 9507, 9502, 9498, 9494, 9489, 9486, 9485, 9484, 9479, 9478, 9477,
9475, 9462, 9462, 9462, 9461, 9459, 9459, 9452, 9449, 9447, 9440, 9430, 9430, 9427, 9426, 9423, 9418, 9414, 9414, 9408,
9401, 9400, 9400, 9396, 9394, 9393, 9392, 9391, 9390, 9389, 9386, 9383, 9378, 9378, 9377, 9374, 9372, 9368, 9368, 9367,
9367, 9365, 9364, 9361, 9359, 9358, 9354, 9351, 9344, 9342, 9339, 9335, 9330, 9320, 9316, 9314, 9312, 9309, 9308, 9304,
9303, 9301, 9301, 9297, 9286, 9283, 9282, 9281, 9278, 9277, 9273, 9272, 9268, 9267, 9263, 9263, 9253, 9253, 9249, 9247,
9246, 9246, 9243, 9241, 9238, 9238, 9237, 9233, 9232, 9230, 9229, 9228, 9226, 9226, 9225, 9221, 9220, 9218, 9218, 9218,
9217, 9216, 9215, 9214, 9212, 9199, 9188, 9187, 9187, 9187, 9184, 9183, 9173, 9172, 9165, 9165, 9164, 9157, 9157, 9156,
9154, 9145, 9142, 9139, 9139, 9135, 9132, 9127, 9125, 9124, 9114, 9114, 9112, 9109, 9102, 9100, 9095, 9091, 9086, 9085,
9084, 9072, 9070, 9065, 9064, 9063, 9055, 9051, 9048, 9043, 9036, 9035, 9034, 9032, 9031, 9031, 9028, 9028, 9023, 9017,
9017, 9016, 9015, 9015, 9014, 9012, 9011, 9005, 8997, 8993, 8992, 8983, 8982, 8982, 8980, 8978, 8978, 8971, 8968, 8967,
8963, 8959, 8956, 8955, 8952, 8946, 8945, 8943, 8943, 8939, 8937, 8937, 8933, 8932, 8932, 8931, 8929, 8927, 8927, 8923,
8922, 8918, 8912, 8911, 8902, 8899, 8896, 8892, 8892, 8880, 8877, 8873, 8873, 8870, 8870, 8869, 8851, 8846, 8842, 8840,
8839, 8838, 8838, 8836, 8834, 8832, 8832, 8832, 8827, 8827, 8826, 8825, 8823, 8816, 8811, 8808, 8808, 8806, 8798, 8797,
8797, 8797, 8796, 8788, 8788, 8781, 8780, 8775, 8773, 8772, 8767, 8767, 8761, 8755, 8755, 8749, 8748, 8748, 8748, 8746,
8744, 8742, 8739, 8737, 8736, 8734, 8734, 8734, 8733, 8725, 8725, 8724, 8724, 8723, 8718, 8717, 8715, 8714, 8714, 8713,
8710, 8706, 8705, 8690, 8689, 8689, 8683, 8680, 8678, 8677, 8672, 8672, 8671, 8668, 8667, 8666, 8664, 8661, 8657, 8655,
8655, 8652, 8650, 8650, 8649, 8646, 8643, 8642, 8642, 8641, 8639, 8636, 8626, 8625, 8623, 8619, 8619, 8619, 8613, 8611,
8605, 8602, 8600, 8600, 8597, 8594, 8589, 8589, 8588, 8588, 8586, 8580, 8578, 8570, 8568, 8566, 8565, 8563, 8562, 8562,
8561, 8560, 8560, 8560, 8558, 8554, 8551, 8549, 8549, 8544, 8543, 8535, 8532, 8528, 8528, 8524, 8524, 8520, 8520, 8517,
8516, 8513, 8513, 8513, 8512, 8509, 8508, 8505, 8504, 8502, 8499, 8498, 8494, 8494, 8492, 8490, 8484, 8479, 8479, 8477,
8477, 8473, 8473, 8467, 8467, 8462, 8460, 8452, 8451, 8449, 8449, 8449, 8447, 8441, 8441, 8436, 8435, 8435, 8434, 8428,
8427, 8427, 8425, 8425, 8422, 8420, 8413, 8412, 8411, 8408, 8408, 8407, 8407, 8405, 8405, 8403, 8398, 8397, 8393, 8388,
8388, 8384, 8384, 8384, 8383, 8381, 8380, 8379, 8377, 8376, 8375, 8375, 8374, 8370, 8369, 8369, 8365, 8365, 8365, 8362,
8353, 8347, 8346, 8342, 8341, 8339, 8338, 8338, 8336, 8335, 8335, 8331, 8330, 8325, 8322, 8322, 8317, 8312, 8311, 8308,
8307, 8306, 8305, 8305, 8302, 8297, 8296, 8292, 8291, 8290, 8287, 8287, 8285, 8285, 8283, 8281, 8271, 8270, 8267, 8265,
8263, 8260, 8257, 8256, 8255, 8252, 8252, 8249, 8249, 8248, 8237, 8233, 8233, 8233, 8230, 8229, 8227, 8227, 8224, 8222,
8222, 8220, 8219, 8213, 8213, 8211, 8210, 8208, 8203, 8199, 8198, 8198, 8196, 8194, 8192, 8192, 8191, 8190, 8184, 8183,
8182, 8182, 8180, 8176, 8176, 8170, 8167, 8165, 8161, 8161, 8161, 8158, 8146, 8145, 8145, 8142, 8137, 8132, 8117, 8113,
8112, 8112, 8108, 8105, 8104, 8103, 8097, 8096, 8096, 8093, 8089, 8087, 8087, 8085, 8084, 8075, 8072, 8072, 8068, 8068,
8065, 8061, 8061, 8059, 8058, 8056, 8055, 8054, 8052, 8051, 8049, 8049, 8047, 8045, 8045, 8043, 8043, 8043, 8042, 8041,
8038, 8037, 8036, 8035, 8032, 8032, 8031, 8028, 8028, 8024, 8022, 8020, 8017, 8016, 8015, 8009, 8002, 8002, 7994, 7994,
7991, 7990, 7989, 7988, 7988, 7987, 7983, 7983, 7979, 7976, 7975, 7970, 7968, 7957, 7956, 7956, 7956, 7955, 7949, 7949,
7945, 7945, 7942, 7938, 7937, 7936, 7935, 7931, 7928, 7927, 7926, 7922, 7916, 7909, 7908, 7907, 7905, 7903, 7903, 7899,
7898, 7898, 7896, 7894, 7891, 7891, 7884, 7884, 7883, 7880, 7877, 7877, 7875, 7873, 7872, 7872, 7872, 7870, 7868, 7862,
7862, 7862, 7862, 7859, 7854, 7846, 7845, 7845, 7845, 7844, 7841, 7839, 7839, 7837, 7836, 7830, 7830, 7820, 7817, 7816,
7816, 7814, 7812, 7805, 7803, 7802, 7801, 7801, 7801, 7794, 7794, 7794, 7793, 7790, 7789, 7786, 7785, 7783, 7779, 7777,
7776, 7773, 7771, 7771, 7767, 7767, 7767, 7764, 7763, 7763, 7762, 7762, 7761, 7756, 7754, 7753, 7753, 7750, 7747, 7744,
7743, 7741, 7741, 7740, 7735, 7735, 7733, 7729, 7729, 7728, 7728, 7726, 7725, 7724, 7721, 7720, 7718, 7714, 7714, 7714,
7712, 7710, 7709, 7708, 7708, 7704, 7703, 7703, 7698, 7695, 7687, 7687, 7682, 7680, 7680, 7678, 7677, 7675, 7675, 7673,
7673, 7668, 7667, 7667, 7666, 7659, 7658, 7656, 7653, 7653, 7651, 7649, 7648, 7644, 7642, 7642, 7641, 7641, 7638, 7636,
7634, 7629, 7629, 7623, 7622, 7621, 7619, 7615, 7605, 7604, 7604, 7604, 7595, 7595, 7592, 7591, 7589, 7588, 7586, 7585,
7585, 7579, 7577, 7577, 7577, 7574, 7568, 7565, 7564, 7562, 7560, 7556, 7556, 7555, 7551, 7548, 7544, 7544, 7541, 7541,
7538, 7537, 7536, 7536, 7536, 7535, 7532, 7529, 7526, 7526, 7525, 7521, 7521, 7516, 7515, 7513, 7512, 7512, 7511, 7510,
7509, 7509, 7508, 7505, 7505, 7503, 7501, 7498, 7496, 7495, 7488, 7488, 7483, 7481, 7479, 7479, 7479, 7474, 7468, 7464,
7461, 7459, 7456, 7456, 7453, 7451, 7451, 7445, 7445, 7444, 7440, 7439, 7438, 7435, 7427, 7427, 7424, 7419, 7415, 7411,
7407, 7407, 7407, 7402, 7395, 7394, 7393, 7393, 7391, 7389, 7386, 7384, 7384, 7382, 7380, 7380, 7379, 7377, 7376, 7375,
7375, 7365, 7365, 7365, 7364, 7363, 7362, 7357, 7352, 7350, 7348, 7346, 7343, 7341, 7341, 7333, 7331, 7329, 7328, 7327,
7327, 7324, 7324, 7323, 7321, 7320, 7319, 7318, 7317, 7316, 7315, 7315, 7312, 7311, 7311, 7308, 7306, 7305, 7305, 7300,
7298, 7297, 7297, 7294, 7293, 7292, 7285, 7283, 7282, 7281, 7279, 7276, 7276, 7271, 7266, 7263, 7262, 7260, 7255, 7255,
7254, 7251, 7248, 7248, 7247, 7247, 7246, 7236, 7236, 7234, 7233, 7231, 7230, 7226, 7224, 7221, 7221, 7220, 7219, 7219,
7218, 7218, 7216, 7216, 7213, 7212, 7208, 7207, 7206, 7206, 7206, 7203, 7199, 7195, 7191, 7190, 7188, 7187, 7187, 7185,
7184, 7183, 7181, 7181, 7180, 7178, 7170, 7164, 7161, 7154, 7154, 7145, 7139, 7138, 7138, 7134, 7134, 7133, 7131, 7127,
7123, 7122, 7122, 7122, 7121, 7120, 7117, 7116, 7115, 7113, 7112, 7109, 7107, 7106, 7105, 7096, 7096, 7093, 7093, 7083,
7083, 7081, 7079, 7078, 7077, 7077, 7074, 7073, 7073, 7066, 7062, 7062, 7062, 7061, 7059, 7056, 7055, 7055, 7053, 7052,
7051, 7051, 7046, 7045, 7045, 7044, 7042, 7040, 7039, 7037, 7036, 7036, 7033, 7032, 7032, 7032, 7031, 7028, 7027, 7023,
7021, 7021, 7016, 7015, 7012, 7010, 7010, 7006, 7005, 7003, 7002, 7002, 7002, 6998, 6996, 6992, 6992, 6991, 6985, 6977,
6976, 6975, 6974, 6973, 6971, 6969, 6968, 6968, 6968, 6963, 6959, 6958, 6957, 6956, 6955, 6954, 6953, 6952, 6952, 6951,
6950, 6950, 6947, 6945, 6942, 6941, 6941, 6940, 6937, 6930, 6927, 6926, 6926, 6923, 6923, 6921, 6918, 6917, 6917, 6915,
6907, 6906, 6904, 6903, 6899, 6898, 6897, 6896, 6896, 6895, 6891, 6890, 6890, 6887, 6884, 6881, 6880, 6880, 6877, 6877,
6876, 6876, 6873, 6872, 6871, 6869, 6869, 6867, 6865, 6860, 6859, 6854, 6852, 6852, 6851, 6850, 6848, 6846, 6846, 6845,
6840, 6840, 6837, 6836, 6836, 6836, 6835, 6835, 6833, 6833, 6831, 6830, 6828, 6827, 6824, 6824, 6820, 6819, 6818, 6817,
6817, 6817, 6816, 6815, 6814, 6811, 6810, 6807, 6807, 6805, 6805, 6804, 6802, 6801, 6801, 6798, 6797, 6793, 6793, 6791,
6790, 6788, 6782, 6774, 6769, 6768, 6762, 6760, 6757, 6755, 6753, 6753, 6751, 6747, 6746, 6743, 6741, 6740, 6738, 6734,
6734, 6732, 6731, 6731, 6729, 6728, 6723, 6721, 6721, 6719, 6718, 6718, 6718, 6716, 6716, 6712, 6711, 6710, 6710, 6707,
6705, 6699, 6690, 6686, 6685, 6682, 6677, 6676, 6675, 6675, 6675, 6672, 6672, 6672, 6670, 6667, 6666, 6661, 6661, 6658,
6657, 6657, 6656, 6655, 6653, 6653, 6649, 6649, 6649, 6646, 6645, 6645, 6641, 6640, 6638, 6631, 6631, 6628, 6627, 6626,
6626, 6623, 6622, 6621, 6619, 6619, 6618, 6617, 6617, 6615, 6615, 6611, 6611, 6609, 6608, 6604, 6600, 6598, 6596, 6595,
6593, 6593, 6590, 6590, 6586, 6585, 6585, 6585, 6584, 6577, 6577, 6577, 6576, 6576, 6574, 6574, 6573, 6571, 6570, 6570,
6566, 6566, 6566, 6566, 6564, 6564, 6563, 6563, 6558, 6557, 6554, 6554, 6553, 6552, 6550, 6550, 6547, 6547, 6543, 6538,
6538, 6535, 6534, 6532, 6528, 6528, 6526, 6525, 6525, 6523, 6523, 6520, 6517, 6514, 6504, 6503, 6499, 6498, 6498, 6494,
6494, 6494, 6493, 6490, 6489, 6487, 6487, 6485, 6484, 6482, 6481, 6480, 6479, 6469, 6468, 6468, 6468, 6464, 6464, 6463,
6463, 6461, 6459, 6457, 6454, 6454, 6453, 6452, 6451, 6449, 6446, 6445, 6445, 6443, 6440, 6437, 6434, 6433, 6425, 6422,
6422, 6420, 6419, 6417, 6412, 6408, 6408, 6407, 6406, 6401, 6399, 6396, 6393, 6388, 6387, 6386, 6385, 6383, 6381, 6380,
6380, 6380, 6379, 6379, 6378, 6377, 6377, 6375, 6369, 6368, 6367, 6365, 6362, 6358, 6356, 6356, 6355, 6354, 6352, 6348,
6348, 6346, 6345, 6345, 6345, 6341, 6339, 6339, 6338, 6338, 6335, 6332, 6331, 6331, 6330, 6326, 6324, 6322, 6321, 6320,
6319, 6317, 6316, 6316, 6316, 6312, 6308, 6307, 6305, 6305, 6305, 6304, 6303, 6301, 6299, 6297, 6295, 6294, 6294, 6290,
6288, 6288, 6286, 6282, 6281, 6281, 6281, 6280, 6280, 6275, 6274, 6274, 6273, 6273, 6271, 6270, 6269, 6269, 6267, 6259,
6253, 6252, 6252, 6250, 6250, 6249, 6248, 6247, 6240, 6240, 6240, 6239, 6238, 6237, 6237, 6235, 6234, 6233, 6233, 6233,
6231, 6225, 6223, 6223, 6222, 6219, 6218, 6218, 6218, 6214, 6214, 6213, 6211, 6211, 6211, 6208, 6208, 6208, 6207, 6206,
6206, 6206, 6205, 6203, 6202, 6201, 6199, 6199, 6198, 6197, 6195, 6195, 6195, 6189, 6188, 6184, 6180, 6178, 6176, 6174,
6173, 6172, 6170, 6169, 6168, 6168, 6168, 6168, 6165, 6165, 6163, 6161, 6159, 6158, 6158, 6157, 6155, 6154, 6153, 6149,
6147, 6146, 6145, 6143, 6142, 6142, 6142, 6142, 6139, 6138, 6138, 6135, 6134, 6134, 6132, 6130, 6130, 6127, 6127, 6123,
6122, 6118, 6117, 6115, 6112, 6112, 6111, 6109, 6109, 6109, 6108, 6108, 6107, 6106, 6104, 6100, 6100, 6100, 6100, 6099,
6099, 6097, 6096, 6096, 6095, 6093, 6093, 6093, 6092, 6087, 6087, 6086, 6080, 6079, 6075, 6074, 6073, 6072, 6072, 6072,
6068, 6066, 6063, 6063, 6062, 6056, 6056, 6055, 6053, 6052, 6050, 6046, 6043, 6043, 6040, 6040, 6040, 6032, 6031, 6031,
6030, 6030, 6028, 6025, 6025, 6021, 6021, 6021, 6021, 6017, 6017, 6007, 6006, 6006, 6005, 6005, 6004, 6002, 6000, 5998,
5998, 5998, 5996, 5995, 5994, 5992, 5991, 5987, 5987, 5986, 5985, 5984, 5983, 5983, 5983, 5982, 5982, 5981, 5979, 5978,
5977, 5975, 5975, 5975, 5973, 5972, 5971, 5970, 5969, 5966, 5965, 5965, 5963, 5962, 5960, 5960, 5956, 5954, 5953, 5952,
5952, 5952, 5951, 5948, 5947, 5946, 5946, 5945, 5943, 5940, 5939, 5937, 5936, 5933, 5932, 5931, 5931, 5930, 5928, 5927,
5927, 5924, 5921, 5918, 5918, 5913, 5909, 5905, 5905, 5903, 5901, 5901, 5901, 5901, 5900, 5899, 5898, 5895, 5893, 5891,
5890, 5889, 5888, 5887, 5886, 5884, 5884, 5881, 5878, 5878, 5875, 5875, 5874, 5873, 5871, 5869, 5869, 5867, 5866, 5865,
5856, 5854, 5853, 5850, 5848, 5848, 5848, 5844, 5843, 5841, 5840, 5840, 5839, 5836, 5836, 5836, 5834, 5831, 5831, 5829,
5827, 5823, 5821, 5820, 5816, 5816, 5813, 5812, 5805, 5804, 5804, 5802, 5801, 5801, 5801, 5799, 5798, 5797, 5795, 5791,
5790, 5790, 5789, 5785, 5785, 5783, 5782, 5781, 5775, 5775, 5774, 5773, 5767, 5764, 5759, 5755, 5744, 5744, 5743, 5742,
5740, 5739, 5738, 5737, 5737, 5735, 5734, 5733, 5731, 5731, 5730, 5729, 5729, 5728, 5727, 5727, 5726, 5726, 5724, 5723,
5720, 5720, 5718, 5717, 5715, 5715, 5714, 5714, 5707, 5703, 5702, 5702, 5702, 5701, 5697, 5696, 5691, 5689, 5688, 5686,
5684, 5683, 5676, 5672, 5669, 5669, 5665, 5665, 5663, 5662, 5655, 5655, 5654, 5653, 5653, 5650, 5647, 5645, 5644, 5641,
5639, 5638, 5637, 5637, 5636, 5634, 5634, 5632, 5631, 5630, 5629, 5627, 5625, 5622, 5618, 5617, 5614, 5614, 5613, 5612,
5612, 5610, 5610, 5609, 5609, 5608, 5607, 5606, 5604, 5601, 5601, 5600, 5599, 5598, 5598, 5597, 5593, 5591, 5589, 5588,
5585, 5582, 5581, 5581, 5579, 5579, 5579, 5578, 5577, 5577, 5574, 5574, 5573, 5573, 5572, 5572, 5568, 5568, 5566, 5565,
5563, 5562, 5561, 5561, 5561, 5560, 5558, 5557, 5554, 5551, 5551, 5551, 5549, 5548, 5547, 5547, 5545, 5544, 5543, 5541,
5540, 5539, 5539, 5538, 5538, 5536, 5532, 5525, 5525, 5524, 5524, 5521, 5521, 5518, 5517, 5513, 5513, 5510, 5509, 5505,
5498, 5497, 5495, 5494, 5492, 5490, 5488, 5487, 5485, 5485, 5485, 5483, 5482, 5479, 5478, 5476, 5474, 5474, 5471, 5471,
5471, 5468, 5468, 5466, 5464, 5464, 5463, 5463, 5462, 5460, 5460, 5460, 5460, 5460, 5459, 5459, 5458, 5456, 5456, 5456,
5449, 5449, 5447, 5446, 5446, 5445, 5441, 5441, 5439, 5437, 5437, 5434, 5433, 5431, 5430, 5430, 5430, 5428, 5427, 5427,
5426, 5426, 5425, 5423, 5422, 5421, 5421, 5419, 5418, 5418, 5417, 5416, 5415, 5414, 5411, 5410, 5405, 5404, 5402, 5396,
5395, 5392, 5392, 5392, 5390, 5390, 5389, 5388, 5386, 5386, 5384, 5381, 5379, 5379, 5378, 5377, 5377, 5376, 5376, 5375,
5374, 5373, 5373, 5372, 5370, 5369, 5369, 5369, 5369, 5368, 5368, 5367, 5367, 5365, 5365, 5363, 5362, 5361, 5359, 5358,
5358, 5357, 5355, 5354, 5353, 5349, 5347, 5347, 5347, 5346, 5345, 5344, 5343, 5343, 5342, 5339, 5338, 5334, 5332, 5328,
5328, 5326, 5324, 5323, 5321, 5320, 5320, 5317, 5317, 5316, 5315, 5315, 5315, 5314, 5314, 5313, 5311, 5308, 5306, 5305,
5305, 5303, 5302, 5301, 5298, 5293, 5290, 5289, 5289, 5286, 5283, 5282, 5280, 5280, 5279, 5276, 5274, 5271, 5269, 5263,
5262, 5261, 5260, 5258, 5257, 5256, 5256, 5252, 5252, 5248, 5248, 5247, 5246, 5245, 5244, 5243, 5241, 5241, 5240, 5239,
5235, 5232, 5232, 5232, 5228, 5226, 5225, 5225, 5223, 5222, 5221, 5221, 5220, 5216, 5215, 5214, 5214, 5214, 5207, 5206,
5206, 5206, 5205, 5203, 5202, 5198, 5198, 5196, 5194, 5187, 5183, 5181, 5180, 5178, 5176, 5175, 5171, 5171, 5168, 5168,
5166, 5165, 5164, 5163, 5161, 5159, 5157, 5157, 5153, 5152, 5150, 5149, 5149, 5149, 5149, 5149, 5148, 5146, 5144, 5142,
5139, 5138, 5133, 5131, 5131, 5131, 5130, 5129, 5127, 5127, 5126, 5126, 5126, 5125, 5123, 5123, 5122, 5121, 5119, 5118,
5115, 5115, 5112, 5112, 5110, 5109, 5108, 5104, 5103, 5102, 5102, 5100, 5100, 5100, 5097, 5095, 5093, 5093, 5092, 5092,
5092, 5091, 5091, 5089, 5088, 5088, 5087, 5085, 5080, 5078, 5070, 5069, 5066, 5066, 5062, 5062, 5062, 5060, 5058, 5056,
5055, 5054, 5054, 5051, 5051, 5050, 5049, 5049, 5048, 5046, 5046, 5044, 5042, 5040, 5040, 5039, 5035, 5035, 5035, 5032,
5032, 5030, 5029, 5028, 5026, 5025, 5023, 5022, 5019, 5016, 5013, 5007, 5007, 5007, 5006, 5005, 5001, 5001, 4999, 4999,
4998, 4995, 4994, 4994, 4990, 4990, 4985, 4985, 4983, 4983, 4982, 4979, 4979, 4979, 4975, 4975, 4975, 4973, 4972, 4971,
4971, 4965, 4965, 4964, 4964, 4963, 4962, 4962, 4960, 4957, 4957, 4954, 4950, 4950, 4947, 4945, 4944, 4942, 4941, 4941,
4941, 4940, 4939, 4938, 4938, 4936, 4932, 4931, 4928, 4927, 4923, 4923, 4922, 4921, 4918, 4918, 4918, 4915, 4915, 4915,
4911, 4911, 4907, 4906, 4899, 4899, 4898, 4898, 4893, 4892, 4891, 4887, 4883, 4883, 4883, 4881, 4877, 4877, 4876, 4874,
4872, 4871, 4871, 4868, 4868, 4868, 4866, 4865, 4865, 4864, 4862, 4862, 4859, 4854, 4852, 4851, 4850, 4850, 4850, 4850,
4845, 4845, 4844, 4843, 4838, 4836, 4836, 4835, 4830, 4829, 4828, 4828, 4827, 4824, 4824, 4823, 4823, 4823, 4819, 4818,
4818, 4818, 4818, 4818, 4816, 4811, 4810, 4809, 4809, 4806, 4805, 4805, 4805, 4804, 4804, 4803, 4802, 4801, 4801, 4799,
4798, 4795, 4794, 4793, 4791, 4790, 4789, 4787, 4786, 4785, 4784, 4783, 4782, 4782, 4782, 4778, 4778, 4777, 4776, 4776,
4776, 4774, 4772, 4771, 4768, 4767, 4766, 4765, 4765, 4763, 4759, 4759, 4757, 4756, 4756, 4756, 4756, 4755, 4754, 4752,
4749, 4749, 4748, 4747, 4746, 4744, 4742, 4742, 4740, 4740, 4737, 4736, 4735, 4733, 4731, 4729, 4729, 4727, 4724, 4721,
4718, 4718, 4716, 4715, 4714, 4713, 4713, 4710, 4710, 4708, 4707, 4707, 4706, 4705, 4705, 4705, 4703, 4701, 4700, 4699,
4699, 4699, 4698, 4698, 4696, 4696, 4695, 4695, 4695, 4694, 4692, 4691, 4691, 4687, 4684, 4681, 4681, 4680, 4679, 4674,
4673, 4671, 4670, 4668, 4665, 4662, 4661, 4661, 4660, 4657, 4657, 4656, 4656, 4653, 4653, 4652, 4652, 4652, 4649, 4649,
4645, 4643, 4643, 4640, 4640, 4640, 4639, 4639, 4631, 4631, 4630, 4628, 4628, 4627, 4627, 4624, 4623, 4623, 4621, 4619,
4619, 4618, 4617, 4617, 4615, 4615, 4611, 4610, 4608, 4608, 4607, 4604, 4603, 4603, 4601, 4598, 4597, 4596, 4596, 4596,
4594, 4589, 4587, 4586, 4585, 4585, 4583, 4583, 4583, 4580, 4579, 4579, 4579, 4579, 4578, 4578, 4577, 4577, 4577, 4575,
4574, 4574, 4571, 4570, 4570, 4569, 4569, 4565, 4562, 4562, 4562, 4559, 4559, 4557, 4557, 4555, 4554, 4554, 4553, 4551,
4551, 4550, 4550, 4549, 4546, 4542, 4542, 4540, 4540, 4540, 4540, 4540, 4539, 4536, 4536, 4532, 4532, 4532, 4530, 4529,
4528, 4527, 4525, 4524, 4524, 4524, 4524, 4523, 4522, 4521, 4520, 4518, 4517, 4516, 4515, 4514, 4513, 4512, 4510, 4509,
4509, 4509, 4509, 4507, 4506, 4504, 4502, 4494, 4494, 4494, 4493, 4492, 4491, 4491, 4490, 4490, 4486, 4484, 4483, 4483,
4481, 4477, 4476, 4476, 4476, 4475, 4474, 4473, 4471, 4471, 4469, 4469, 4468, 4468, 4467, 4467, 4466, 4465, 4464, 4464,
4464, 4462, 4461, 4460, 4460, 4458, 4454, 4453, 4453, 4452, 4450, 4450, 4449, 4449, 4448, 4447, 4446, 4445, 4443, 4442,
4441, 4439, 4437, 4437, 4432, 4432, 4431, 4431, 4428, 4428, 4428, 4426, 4426, 4424, 4422, 4422, 4420, 4418, 4415, 4415,
4415, 4415, 4415, 4412, 4412, 4412, 4411, 4408, 4407, 4405, 4404, 4403, 4403, 4402, 4401, 4400, 4400, 4399, 4399, 4396,
4394, 4392, 4391, 4390, 4389, 4386, 4385, 4384, 4384, 4382, 4381, 4381, 4381, 4377, 4376, 4376, 4375, 4373, 4373, 4373,
4373, 4373, 4371, 4370, 4370, 4369, 4369, 4368, 4367, 4367, 4367, 4367, 4366, 4365, 4365, 4364, 4364, 4362, 4361, 4361,
4360, 4359, 4358, 4355, 4354, 4354, 4353, 4352, 4350, 4347, 4346, 4343, 4341, 4340, 4338, 4336, 4336, 4332, 4331, 4329,
4328, 4324, 4323, 4323, 4320, 4320, 4320, 4320, 4319, 4319, 4317, 4317, 4316, 4315, 4312, 4311, 4309, 4309, 4305, 4305,
4304, 4303, 4301, 4301, 4298, 4297, 4293, 4291, 4290, 4290, 4287, 4286, 4286, 4286, 4286, 4283, 4282, 4282, 4278, 4278,
4276, 4270, 4267, 4265, 4263, 4261, 4261, 4261, 4259, 4259, 4256, 4256, 4251, 4250, 4249, 4248, 4248, 4244, 4243, 4240,
4240, 4240, 4240, 4239, 4237, 4237, 4236, 4235, 4234, 4233, 4233, 4232, 4231, 4229, 4226, 4225, 4221, 4219, 4218, 4218,
4215, 4214, 4213, 4213, 4213, 4210, 4210, 4210, 4210, 4210, 4209, 4207, 4206, 4206, 4206, 4206, 4204, 4202, 4201, 4201,
4199, 4199, 4197, 4197, 4195, 4195, 4195, 4194, 4194, 4193, 4192, 4191, 4191, 4189, 4189, 4188, 4187, 4186, 4184, 4184,
4176, 4174, 4172, 4171, 4171, 4170, 4170, 4170, 4170, 4169, 4169, 4169, 4168, 4168, 4167, 4163, 4163, 4161, 4161, 4161,
4160, 4160, 4159, 4158, 4158, 4157, 4157, 4157, 4156, 4153, 4152, 4151, 4151, 4151, 4151, 4151, 4150, 4149, 4148, 4147,
4146, 4146, 4144, 4141, 4140, 4138, 4137, 4135, 4133, 4131, 4130, 4130, 4128, 4127, 4127, 4127, 4126, 4124, 4123, 4122,
4122, 4121, 4121, 4121, 4119, 4119, 4119, 4119, 4118, 4115, 4115, 4115, 4114, 4112, 4110, 4108, 4108, 4108, 4107, 4106,
4105, 4104, 4104, 4103, 4102, 4100, 4099, 4098, 4093, 4091, 4090, 4089, 4088, 4087, 4086, 4085, 4085, 4085, 4085, 4085,
4084, 4083, 4082, 4082, 4081, 4081, 4081, 4079, 4079, 4079, 4077, 4076, 4075, 4075, 4074, 4073, 4072, 4072, 4070, 4069,
4067, 4066, 4066, 4064, 4063, 4063, 4063, 4063, 4062, 4061, 4061, 4060, 4059, 4057, 4056, 4056, 4055, 4055, 4053, 4051,
4051, 4050, 4049, 4049, 4049, 4043, 4042, 4041, 4040, 4040, 4040, 4040, 4038, 4036, 4036, 4034, 4033, 4033, 4032, 4032,
4031, 4029, 4029, 4029, 4029, 4027, 4026, 4023, 4021, 4021, 4021, 4020, 4018, 4016, 4016, 4015, 4014, 4013, 4013, 4013,
4011, 4008, 4007, 4006, 4005, 4002, 4002, 4002, 4002, 4001, 3998, 3996, 3995, 3994, 3993, 3993, 3993, 3991, 3990, 3987,
3987, 3987, 3986, 3983, 3983, 3981, 3980, 3979, 3979, 3977, 3976, 3971, 3970, 3969, 3968, 3968, 3968, 3968, 3965, 3965,
3964, 3963, 3961, 3960, 3960, 3959, 3958, 3956, 3956, 3955, 3955, 3953, 3953, 3953, 3952, 3952, 3951, 3951, 3950, 3950,
3949, 3949, 3946, 3945, 3944, 3943, 3943, 3943, 3943, 3941, 3941, 3937, 3935, 3934, 3934, 3934, 3932, 3931, 3930, 3928,
3926, 3926, 3925, 3924, 3924, 3923, 3923, 3921, 3921, 3918, 3915, 3915, 3915, 3915, 3911, 3911, 3910, 3910, 3910, 3907,
3907, 3907, 3904, 3902, 3901, 3901, 3901, 3900, 3899, 3899, 3896, 3895, 3894, 3894, 3893, 3890, 3889, 3889, 3886, 3883,
3883, 3881, 3880, 3880, 3879, 3878, 3878, 3877, 3877, 3877, 3877, 3875, 3873, 3873, 3873, 3872, 3871, 3870, 3869, 3868,
3867, 3865, 3865, 3865, 3864, 3862, 3862, 3862, 3860, 3858, 3858, 3857, 3854, 3853, 3849, 3848, 3847, 3846, 3846, 3846,
3846, 3839, 3838, 3838, 3837, 3837, 3835, 3833, 3833, 3833, 3830, 3828, 3828, 3827, 3827, 3826, 3825, 3825, 3825, 3824,
3824, 3822, 3820, 3819, 3818, 3818, 3812, 3811, 3810, 3809, 3807, 3807, 3807, 3806, 3805, 3805, 3803, 3802, 3802, 3801,
3799, 3799, 3799, 3799, 3797, 3795, 3795, 3793, 3792, 3791, 3791, 3790, 3790, 3790, 3788, 3787, 3787, 3786, 3785, 3783,
3783, 3782, 3781, 3780, 3780, 3777, 3776, 3773, 3771, 3771, 3770, 3767, 3767, 3766, 3766, 3766, 3765, 3762, 3762, 3755,
3752, 3752, 3750, 3748, 3748, 3748, 3746, 3746, 3745, 3745, 3744, 3744, 3743, 3742, 3741, 3739, 3739, 3735, 3735, 3734,
3734, 3733, 3731, 3731, 3729, 3729, 3727, 3727, 3726, 3725, 3723, 3723, 3723, 3722, 3719, 3718, 3717, 3715, 3713, 3713,
3713, 3713, 3710, 3710, 3710, 3710, 3709, 3708, 3707, 3707, 3706, 3706, 3706, 3705, 3705, 3703, 3702, 3701, 3697, 3696,
3696, 3695, 3694, 3694, 3692, 3691, 3691, 3690, 3689, 3689, 3688, 3688, 3688, 3688, 3687, 3684, 3681, 3680, 3680, 3680,
3679, 3678, 3678, 3677, 3676, 3676, 3674, 3672, 3672, 3672, 3671, 3668, 3668, 3667, 3666, 3660, 3659, 3659, 3658, 3658,
3657, 3655, 3654, 3654, 3654, 3653, 3650, 3650, 3649, 3648, 3646, 3645, 3645, 3644, 3643, 3642, 3641, 3638, 3638, 3637,
3637, 3637, 3637, 3636, 3634, 3634, 3633, 3633, 3633, 3631, 3631, 3630, 3629, 3627, 3627, 3627, 3626, 3626, 3625, 3623,
3622, 3621, 3619, 3619, 3617, 3615, 3615, 3615, 3614, 3614, 3614, 3612, 3612, 3612, 3611, 3609, 3609, 3609, 3607, 3605,
3604, 3604, 3603, 3602, 3601, 3600, 3600, 3598, 3596, 3596, 3594, 3594, 3593, 3593, 3592, 3592, 3591, 3591, 3590, 3589,
3588, 3588, 3587, 3587, 3586, 3585, 3585, 3585, 3583, 3581, 3580, 3579, 3578, 3577, 3577, 3576, 3576, 3574, 3574, 3574,
3573, 3573, 3573, 3572, 3570, 3570, 3570, 3569, 3569, 3567, 3563, 3563, 3562, 3562, 3561, 3559, 3559, 3556, 3555, 3555,
3553, 3553, 3553, 3552, 3551, 3550, 3550, 3549, 3548, 3547, 3547, 3547, 3547, 3545, 3545, 3544, 3543, 3543, 3541, 3540,
3540, 3540, 3540, 3539, 3539, 3539, 3537, 3536, 3536, 3536, 3535, 3531, 3530, 3529, 3528, 3528, 3527, 3527, 3525, 3524,
3524, 3523, 3523, 3521, 3520, 3520, 3519, 3519, 3518, 3518, 3518, 3518, 3517, 3515, 3515, 3513, 3513, 3512, 3512, 3511,
3509, 3509, 3508, 3507, 3507, 3506, 3506, 3506, 3506, 3505, 3505, 3504, 3502, 3502, 3502, 3502, 3500, 3500, 3500, 3498,
3498, 3497, 3495, 3494, 3494, 3494, 3493, 3491, 3490, 3489, 3489, 3488, 3487, 3487, 3483, 3483, 3479, 3477, 3475, 3474,
3472, 3470, 3470, 3469, 3469, 3468, 3468, 3468, 3466, 3465, 3464, 3464, 3461, 3460, 3460, 3460, 3458, 3457, 3457, 3456,
3456, 3455, 3453, 3453, 3452, 3452, 3452, 3452, 3451, 3449, 3449, 3447, 3446, 3445, 3445, 3445, 3445, 3444, 3444, 3444,
3443, 3443, 3443, 3443, 3441, 3440, 3438, 3437, 3434, 3434, 3434, 3433, 3433, 3430, 3430, 3429, 3429, 3427, 3427, 3426,
3425, 3425, 3424, 3424, 3424, 3420, 3415, 3415, 3414, 3414, 3414, 3413, 3413, 3412, 3411, 3409, 3408, 3406, 3405, 3405,
3403, 3403, 3402, 3400, 3400, 3400, 3400, 3400, 3400, 3399, 3396, 3396, 3393, 3393, 3392, 3391, 3390, 3390, 3389, 3388,
3386, 3385, 3384, 3384, 3384, 3380, 3380, 3377, 3377, 3376, 3372, 3371, 3371, 3371, 3370, 3370, 3370, 3369, 3369, 3367,
3366, 3365, 3365, 3364, 3364, 3363, 3362, 3361, 3361, 3360, 3359, 3358, 3355, 3354, 3354, 3354, 3354, 3354, 3353, 3353,
3351, 3351, 3350, 3350, 3350, 3350, 3346, 3344, 3344, 3342, 3341, 3340, 3339, 3338, 3338, 3338, 3338, 3336, 3336, 3335,
3335, 3332, 3331, 3331, 3331, 3330, 3329, 3325, 3325, 3322, 3320, 3320, 3318, 3317, 3317, 3317, 3316, 3315, 3314, 3313,
3312, 3312, 3311, 3310, 3310, 3309, 3309, 3305, 3305, 3304, 3303, 3303, 3301, 3301, 3300, 3300, 3299, 3299, 3297, 3297,
3297, 3293, 3293, 3293, 3293, 3292, 3289, 3289, 3286, 3285, 3285, 3284, 3283, 3282, 3282, 3282, 3282, 3282, 3282, 3280,
3278, 3277, 3277, 3277, 3276, 3276, 3273, 3271, 3271, 3271, 3270, 3269, 3268, 3267, 3267, 3266, 3265, 3264, 3263, 3263,
3263, 3262, 3260, 3260, 3260, 3259, 3259, 3258, 3257, 3257, 3255, 3254, 3251, 3249, 3248, 3248, 3247, 3245, 3244, 3244,
3244, 3241, 3241, 3241, 3240, 3238, 3237, 3236, 3236, 3234, 3233, 3232, 3232, 3229, 3229, 3229, 3229, 3228, 3227, 3226 };
		Word new_word;
		switch (operation) {
		case WordLibOperation::GenerateRandomWord:
			if (parameter > number_of_words) parameter = number_of_words;
			new_word.index = utils::randomNum(0, parameter - 1);
			new_word.str = word_lib[new_word.index];
			new_word.freq = word_lib_frequency[new_word.index];
			while (new_word.str[0] == '-') {
				new_word.index++;
				new_word.str = word_lib[new_word.index];
				new_word.freq = word_lib_frequency[new_word.index];
			}// ȥ�����Ƴ��Ĵʻ�
			return new_word;
		case WordLibOperation::FindWordByIndex:
			new_word.index = parameter;
			new_word.str = word_lib[parameter];
			if (new_word.str[0] == '-') new_word.str = new_word.str.substr(1); // ȥ������
			new_word.freq = word_lib_frequency[parameter];
			return new_word;
		}

	}
	void newWord() {
		mods = 0;
		word = wordLibrary(WordLibOperation::GenerateRandomWord, HANGMAN_WORD_LIST);
		guess_history = "";
		HP = 7;
	}
	string writeHistory(double score) {
		char tmp[1024];
		sprintf_s(tmp,
			"INSERT INTO game_hangman_history (time,player_id,word_id,word_str,guess_history,mods,score) values (%lld,%lld,%d,\"%s\",\"%s\",%d,%f)",
			time(NULL), player_id, word.index, word.str, guess_history, mods, score);
		
		osucat::Database db;
		db.Connect();
		db.Insert(tmp);
		db.Close();
		return "";
	}
	string leaveRoom(double score) {
		string out = writeHistory(score);
		player_id = 0;
		player_nickname = "";
		return out;
	}
public:
	Hangman() {
		player_id = 0;
		player_nickname = "";
		newWord();
	}
	string guess(char input) {
		if (input <= 'Z' && input >= 'A') input += 'a' - 'A';
		if (input > 'z' || input < 'a') return u8"������Ĳ�����ĸ" + cuteEmoji(CuteEmojiType::Sad);
		if (guess_history.find(input) != guess_history.npos) return u8"���Ѿ��¹������ĸ�ˣ�" + cuteEmoji(CuteEmojiType::Happy);// +"\n" + this->getStatus();
		guess_history += input;
		if (word.str.find(input) != word.str.npos) {
			return this->getStatus();
		}
		else {
			HP -= 1;
			return this->getStatus();//u8"û�������ĸ~(HP=" + std::to_string(HP) + u8")";// + std::to_string(input);
		}
	}
	string getStatus() {
		string output = "";
		string mods_display = "";
		if (mods & HANGMAN_MOD_E) mods_display += "E";
		if (mods & HANGMAN_MOD_H) mods_display += "H";
		if (mods & HANGMAN_MOD_D) mods_display += "D";
		if (mods & HANGMAN_MOD_I) mods_display += "I";
		if (mods_display != "") mods_display = "+" + mods_display + " ";
		for (int i = 0; i < word.str.length(); i++) {
			if ((mods & HANGMAN_MOD_I) && (i == 0)) {
				//����ʾ��һ����ĸ��λ��
			}
			else {
				if (guess_history.find(word.str[i]) != guess_history.npos) {
					if (mods & HANGMAN_MOD_D) {
						output = output + word.str[i];
					}
					else {
						output = output + word.str[i] + u8" ";
					}
				}
				else {
					if (mods & HANGMAN_MOD_D) {
						//����ʾ����
					}
					else if (mods & HANGMAN_MOD_H) {
						bool str_ends = true;
						for (int j = i + 1; j < word.str.length(); j++) {
							if (guess_history.find(word.str[j]) != guess_history.npos) {
								str_ends = false;
								break;
							}
						}
						if (str_ends) {
							break;
						}
						else if (utils::randomNum(0, 99) <= 49) {
							output += u8"  ";
						}
						else {
							output += u8"   ";
						}
					}
					else {
						output += u8"_ ";
					}
				}
			}
		}
		output += u8"\n" + mods_display + "HP:" + std::to_string(HP);
		if (guess_history != "") output += u8" ����:" + guess_history;
		return output;
	}
	string getAnswer() {
		return u8"��ȷ����" + word.str;
	}
	bool isGameEnded() {
		if (HP <= 0) return true;
		for (int i = 0; i < word.str.length(); i++) {
			if (guess_history.find(word.str[i]) == guess_history.npos) return false;
		}
		return true;
	}
	string endGame() {
		string output = "";
		if (HP <= 0) {
			output = u8"�����~" + cuteEmoji(CuteEmojiType::Sad) + u8"\n��������Ȼ�����" +  std::to_string(scoring(word, guess_history, mods)) + u8"��" + "\n" + this->getAnswer();
			output += leaveRoom(scoring(word, guess_history, mods));
		}
		else {
			output = u8"��¶���~��������èè�����" + std::to_string(scoring(word, guess_history, mods)) + u8"��" + cuteEmoji(CuteEmojiType::Happy);
			output += leaveRoom(scoring(word, guess_history, mods));
		}
		return output;
	}
	string giveUp() {
		string output = getAnswer() + u8"\n����������ˣ�����ֻ��" + std::to_string(scoring(word, guess_history, mods)) + u8"Ŷ~";
		output += leaveRoom(scoring(word, guess_history, mods));
		return output;
	}
};

const static int lobby_size = 20;
static Hangman hangman_list[lobby_size];

class HangmanGame {
public:
	static std::string updatePlayer(Target tar) {
		string out = "";
		osucat::Database db;
		db.Connect();
		json j = db.Select("SELECT score, guess_history, word_str, mods FROM game_hangman_history where player_id = " + to_string(tar.user_id));
		
		double max_qq = 0, qq = 0, score = 0;
		const double lambda = 0.98;
		double acc = 0, passrate = 0;
		int playcount = 0, total_guesses = 0, total_hits = 0, total_passes = 0;
		int mods;
		for (int i = 0; i < j.size(); i++) {
			score = stod(j[i]["score"].get<std::string>());
			mods = stoi(j[i]["mods"].get<std::string>());
			qq = qq * lambda + score;
			std::string word_str = j[i]["word_str"].get<std::string>();
			std::string guess_history = j[i]["guess_history"].get<std::string>();

			bool passed = true;
			for (int k = 0; k < word_str.length(); k++) {
				if (guess_history.find(word_str[k]) == guess_history.npos) {
					passed = false;
					break;
				}
			}

			if (mods & HANGMAN_MOD_E) guess_history = guess_history.substr(1); // remove the hint by ez

			for (int k = 0; k < guess_history.length(); k++) {
				if (word_str.find(guess_history[k]) != word_str.npos) {
					total_hits++;
				}
			}

			total_guesses += guess_history.length();
			if (qq > max_qq) max_qq = qq;
			playcount++;
			if (passed) total_passes++;
		}
		if (total_guesses > 0) acc = (double) total_hits / (double) total_guesses;
		if (playcount > 0) passrate = (double)total_passes / (double)playcount;
		
		max_qq *= (1 - lambda) * 100;

		db.Update("UPDATE game_hangman_ranking SET qqpoint = " + std::to_string(max_qq) + ", acc = " + std::to_string(acc) + ", passrate = " + std::to_string(passrate) + ", playcount = " + std::to_string(playcount) + " where player_id = " + std::to_string(tar.user_id));
		json jranking = db.Select("Select qqpoint, player_id, qqrank from game_hangman_ranking ORDER BY qqpoint DESC");
		for (int i = 0; i < jranking.size(); i++) {
			if (jranking[i]["player_id"].get<std::string>() == std::to_string(tar.user_id)) {
				int prev_rank = stoi(jranking[i]["qqrank"].get<std::string>());
				int curr_rank = i + 1;
				if (curr_rank != prev_rank) {
					db.Update("UPDATE game_hangman_ranking SET qqrank = " + std::to_string(curr_rank) + " where player_id = " + std::to_string(tar.user_id));
					if (curr_rank < prev_rank) out = " (+" + std::to_string(prev_rank - curr_rank) + ")";
				}
				break;
			}
		}

		db.Close();
		return out;
	}
	static int findPlayer(int64_t id) {
		for (int i = 0; i < lobby_size; i++) {
			if (hangman_list[i].player_id == id) return i;
		}
		return -1;
	}
	static std::string introduceHangman() { return u8"��һ����µ�����~������èè������һ��δ֪���ʣ���Ҫ����������Щ��ĸ~���������ĸa�Ļ�����!a���ظ���~����ʹ��!�µ��ʰ��� �鿴����" + cuteEmoji(CuteEmojiType::Happy) + "\n"; }

	static bool playerRankingExists(Target tar) {
		osucat::Database db;
		db.Connect();
		json j = db.Select("SELECT player_id, nickname from game_hangman_ranking");
		for (int i = 0; i < j.size(); i++) {
			if (j[i]["player_id"].get<std::string>() == std::to_string(tar.user_id)) {
				if (j[i]["nickname"].get<std::string>() != tar.nickname) {
					db.Update("UPDATE game_hangman_ranking SET nickname = \"" + tar.nickname + "\" where player_id = " + std::to_string(tar.user_id));
				}
				return true;
			}
		}
		char tmp[200];
		sprintf_s(tmp, "INSERT INTO game_hangman_ranking (player_id, nickname, qqpoint, qqrank) values (%lld, \"%s\", %f, %d)", tar.user_id, tar.nickname.c_str(), 0, 0);
		db.Insert(tmp);
		db.Close();
		return false;
	}
	static std::string showPlayerData(Target tar) {
		std::string out = "";
		if (!playerRankingExists(tar)) {
			out = u8"�㻹û�������~";
			return out;
		}
		string rank_change = updatePlayer(tar);
		osucat::Database db;
		db.Connect();
		json j = db.Select("SELECT * from game_hangman_ranking where player_id = " + std::to_string(tar.user_id));
		out = u8"�û�" + j[0]["nickname"].get<std::string>() + " (" + j[0]["player_id"].get<std::string>() + u8")�Ĳµ������ݣ�";
		out += u8"\npp:" + j[0]["qqpoint"].get<std::string>();
		out += u8"\n����:" + std::to_string(std::stoi(j[0]["qqrank"].get<std::string>())) + rank_change;
		out += u8"\n�������:" + std::to_string(std::stoi(j[0]["playcount"].get<std::string>()));
		out += u8"\n������:" + std::to_string(100.0 * std::stod(j[0]["acc"].get<std::string>())) + "%";
		out += u8"\nͨ����:" + std::to_string(100.0 * std::stod(j[0]["passrate"].get<std::string>())) + "%";
		db.Close();
		return out;
	}
	//��ʼһ����Ϸ������
	static string startHangman(Target tar, string cmd) {
		std::string out = "";
		char mods = 0x00;
		if (findPlayer(tar.user_id) != -1) {
			out += u8"�����һ����Ϸ��û�н�����~" + std::to_string(findPlayer(tar.user_id));
			return out;
		}
		if (!playerRankingExists(tar)) {
			out = introduceHangman();
		}
		for (int i = 0; i < min(cmd.length(), 4); i++) {
			if (cmd[i] == 'E' || cmd[i] == 'e') mods |= HANGMAN_MOD_E;
			if (cmd[i] == 'H' || cmd[i] == 'h') mods |= HANGMAN_MOD_H;
			if (cmd[i] == 'D' || cmd[i] == 'd') mods |= HANGMAN_MOD_D + HANGMAN_MOD_H;
			if (cmd[i] == 'I' || cmd[i] == 'i') mods |= HANGMAN_MOD_I;
		}
		for (int i = 0; i < lobby_size; i++) {
			if (hangman_list[i].player_id == 0 || hangman_list[i].start_time - time(NULL) > HANGMAN_EXPIRATION_TIME) {
				if (hangman_list[i].start_time - time(NULL) > HANGMAN_EXPIRATION_TIME) {
					hangman_list[i].leaveRoom(0);//��ɨ�ɾ����������
				}
				hangman_list[i].start_time = time(NULL);
				hangman_list[i].player_id = tar.user_id;
				hangman_list[i].newWord();
				hangman_list[i].mods = mods;
				if (mods & HANGMAN_MOD_E) {
					hangman_list[i].guess(hangman_list[i].word.str[1]);
					hangman_list[i].HP = 8;
				}
				out += hangman_list[i].getStatus();
				return out;
			}
		}
		out = u8"�����Ѿ�����" + cuteEmoji(CuteEmojiType::Sad) + u8"\n�Ȼ��������" + cuteEmoji(CuteEmojiType::Happy);
		return out;
	}
	//������Ϸ������
	static string inputHangman(Target tar, string input) {
		std::string out;
		int position = findPlayer(tar.user_id);
		if (position == -1) {
			return u8"�㻹û�п�ʼһ����Ϸ��" + cuteEmoji(CuteEmojiType::Happy);
		}
		else {
			if (input == "") {
				return u8"��������ǿ�������";
			}
			out = hangman_list[position].guess(input[0]);
			if (hangman_list[position].isGameEnded()) {
				out += "\n" + hangman_list[position].endGame();
				out += updatePlayer(tar);
			}
		}
		return out;
	}
	//����һ����Ϸ
	static string giveupHangman(Target tar) {
		int position = findPlayer(tar.user_id);
		if (position == -1) {
			return u8"�㻹û������Ϸ����" + cuteEmoji(CuteEmojiType::Happy);
		}
		else {
			string out = hangman_list[position].giveUp();
			out += updatePlayer(tar);
			return out; 
		}
	}
	//��÷���״̬���б�
	static std::string gameStatus() {
		std::string out = u8"�����б�\n";
		for (int i = 0; i < lobby_size; i++) {
			if (hangman_list[i].player_id != 0) out += std::to_string(i) + u8":" + std::to_string(hangman_list[i].player_id) + u8"\n" + hangman_list[i].getStatus() + u8"\n";
		}
		return out;
	}
	//������ranking��
	static std::string hangmanRanking(int page) {
		std::string out = "";
		osucat::Database db;
		db.Connect();
		json j = db.Select("SELECT nickname, qqpoint from game_hangman_ranking ORDER BY qqpoint DESC");

		page = max(page, 1);
		for (int i = min(j.size(), (page - 1) * 10); i < min(j.size(), page * 10); i++) {
			out += std::to_string(i + 1) + ": " + std::to_string(stoi(j[i]["qqpoint"].get<std::string>())) + "pp " + j[i]["nickname"].get<std::string>();
			if (i + 1 < min(j.size(), page * 10)) out += "\n";
		}
		db.Close();
		return out;
	}
	static std::string recalculateAllScores() {
		osucat::Database db;
		db.Connect();
		json j = db.Select("Select id,word_id,guess_history,mods from game_hangman_history");
		int jsize = j.size();
		for (int i = 0; i < jsize; i++) {
			int id = std::stoi(j[i]["id"].get<std::string>());
			int word_id = std::stoi(j[i]["word_id"].get<std::string>());
			std::string guess_history = j[i]["guess_history"].get<std::string>();
			char mods = std::stoi(j[i]["mods"].get<std::string>());
			double score = Hangman::scoring(Hangman::wordLibrary(Hangman::WordLibOperation::FindWordByIndex, word_id), guess_history, mods);
			db.Update("UPDATE game_hangman_history SET score = " + std::to_string(score) + " where id = " + std::to_string(id));
		}
		db.Close();
		return u8"�ɹ������¼�����" + std::to_string(jsize) + u8"���ɼ�";
	}
};


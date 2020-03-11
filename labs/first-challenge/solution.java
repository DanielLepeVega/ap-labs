import java.util.*;

public class solution{

	public String strToCheck;

	public solution(){
		this.strToCheck = "";
	}

	public void solve(String input){
		Set<Character> hash_Set = new HashSet<Character>();
		for(int i=0;i<input.length();i++){
			hash_Set.add(input.charAt(i));
		}
		System.out.println("Output: " + hash_Set.size());
	}

	public static void main(String[] args){
		solution sol = new solution();
		sol.solve(args[0]);
	}
}
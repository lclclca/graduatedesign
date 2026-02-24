package ima2c.entity.code;

import java.io.Serializable;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

public class ProgramCall implements Serializable {

	public ProgramCall() {
		super();
	}


	private String methodName;

	private List<String> sourceText = new LinkedList<>();


	private String calledMethodName;

	private String sourceLanguage;


	private List<HashMap<String, String>> subprogramParameters = new LinkedList<>();
	
	private List<String[]> threadParameters = new LinkedList<>();

	private List<String> threadVariables = new LinkedList<>();

	public String getMethodName() {
		return methodName;
	}

	public void setMethodName(String methodName) {
		this.methodName = methodName;
	}

	public List<String> getSourceText() {
		return sourceText;
	}

	public void setSourceText(List<String> sourceText) {
		this.sourceText = sourceText;
	}

	public String getCalledMethodName() {
		return calledMethodName;
	}

	public void setCalledMethodName(String calledMethodName) {
		this.calledMethodName = calledMethodName;
	}

	public String getSourceLanguage() {
		return sourceLanguage;
	}

	public void setSourceLanguage(String sourceLanguage) {
		this.sourceLanguage = sourceLanguage;
	}

	public List<HashMap<String, String>> getSubprogramParameters() {
		return subprogramParameters;
	}

	public void setSubprogramParameters(List<HashMap<String, String>> subprogramParameters) {
		this.subprogramParameters = subprogramParameters;
	}

	public List<String[]> getThreadParameters() {
		return threadParameters;
	}

	public void setThreadParameters(List<String[]> threadParameters) {
		this.threadParameters = threadParameters;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((calledMethodName == null) ? 0 : calledMethodName.hashCode());
		result = prime * result + ((methodName == null) ? 0 : methodName.hashCode());
		result = prime * result + ((sourceLanguage == null) ? 0 : sourceLanguage.hashCode());
		result = prime * result + ((sourceText == null) ? 0 : sourceText.hashCode());
		result = prime * result + ((subprogramParameters == null) ? 0 : subprogramParameters.hashCode());
		result = prime * result + ((threadParameters == null) ? 0 : threadParameters.hashCode());
		result = prime * result + ((threadVariables == null) ? 0 : threadVariables.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		ProgramCall other = (ProgramCall) obj;
		if (calledMethodName == null) {
			if (other.calledMethodName != null)
				return false;
		} else if (!calledMethodName.equals(other.calledMethodName))
			return false;
		if (methodName == null) {
			if (other.methodName != null)
				return false;
		} else if (!methodName.equals(other.methodName))
			return false;
		if (sourceLanguage == null) {
			if (other.sourceLanguage != null)
				return false;
		} else if (!sourceLanguage.equals(other.sourceLanguage))
			return false;
		if (sourceText == null) {
			if (other.sourceText != null)
				return false;
		} else if (!sourceText.equals(other.sourceText))
			return false;
		if (subprogramParameters == null) {
			if (other.subprogramParameters != null)
				return false;
		} else if (!subprogramParameters.equals(other.subprogramParameters))
			return false;
		if (threadParameters == null) {
			if (other.threadParameters != null)
				return false;
		} else if (!threadParameters.equals(other.threadParameters))
			return false;
		if (threadVariables == null) {
			if (other.threadVariables != null)
				return false;
		} else if (!threadVariables.equals(other.threadVariables))
			return false;
		return true;
	}

	public List<String> getThreadVariables() {
		return threadVariables;
	}

	public void setThreadVariables(List<String> threadVariables) {
		this.threadVariables = threadVariables;
	}

	@Override
	public String toString() {
		return "ProgramCall [methodName=" + methodName + ", sourceText=" + sourceText + ", calledMethodName="
				+ calledMethodName + ", sourceLanguage=" + sourceLanguage + ", subprogramParameters="
				+ subprogramParameters + ", threadParameters=" + threadParameters + ", threadVariables="
				+ threadVariables + "]";
	}

//	public List<HashMap<String, String>> getThreadParameters() {
//		return threadParameters;
//	}
//
//	public void setThreadParameters(List<HashMap<String, String>> threadParameters) {
//		this.threadParameters = threadParameters;
//	}

}
